#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cassert>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "CrawlerManager.h"
#include "Common.h"
#include "HtmlParser.h"

#define MAX_MESSAGE_SIZE 512

// ----- Listen Manager

ListenManager::ListenManager( Init init, Frontier *frontier, FileBloomfilter *visited, size_t numListenThreads )
    : CrawlerManager( init, frontier, visited ), connectHandler(
        { "ConnectHandler", numListenThreads, init.printMutex, init.machineID, ThreadPool::TaskPool },
        frontier, visited )
    {
    }

ListenManager::~ListenManager( )
    {
    }

void ListenManager::DoLoop( size_t threadID )
    {
    connectHandler.Start();
    while ( alive )
        {
        try
            {
            int sockfd = startServer( threadID );
            runServer( sockfd, threadID );
            }
        catch ( String e )
            {
            //Print( String("Exception: ") + e, threadID );
            }
        catch ( ... )
            {
            Print("Exception: uncaught", threadID);
            }
        }
    connectHandler.Stop();
    }

int ListenManager::startServer( size_t threadID )
    {
    // (1) Create socket
	int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd == -1 ) 
        throw String("Unable to open stream socket");

	// (2) Set the "reuse port" socket option
	int yesval = 1;
	if ( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof( yesval ) ) == -1) 
        throw String("Unable to set socket options");

	// (3) Create a sockaddr_in struct for the proper port and bind() to it.
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons( PORT );

	// (3b) Bind to the port.
	if ( bind( sockfd, ( sockaddr * ) &addr, sizeof( addr ) ) == -1 ) 
        throw String("Unable to bind stream socket");

	// (3c) Detect which port was chosen.
	socklen_t length = sizeof( addr );
	if ( getsockname( sockfd, ( sockaddr * ) &addr, &length ) == -1 ) 
		throw String("Unable to detect port");
    if ( ntohs( addr.sin_port ) != PORT )
        throw String("Incorrect listen port");
        
    return sockfd;
    }

void ListenManager::runServer( int sockfd, size_t threadID )
    {
	// (4) Begin listening for incoming connections.
	if ( listen( sockfd, QUEUE_SIZE ) == -1 )
        throw String("Socket listen failed");
    
    String output = "Listening on port ";
    output += ltos( PORT );
	Print( output, threadID );

	// (5) Serve incoming connections one by one forever.
	while ( alive ) 
        {
        int connectionfd = accept( sockfd, 0, 0 );
		if ( connectionfd == -1 ) 
            throw String("Unable to accept connection");
        // Add the connection to be handled
        connectHandler.PushTask( (void *) new int(connectionfd), true );
        }
    }

// ---- Connect Handler

ConnectHandler::ConnectHandler( Init init, Frontier *frontier, FileBloomfilter *visited )
    : CrawlerManager( init, frontier, visited )
    {
    MutexInit(&countURLmutex, nullptr);
    }

void ConnectHandler::DoTask( Task task, size_t threadID )
    {
    int *fd =  ( int * ) task.args;
    try
        {
        String message = handleConnect( *fd, threadID );
        //String output = "URL received: ";
        //output += message;
        //Print( output, threadID );
        incrementCountURL( threadID );
        // Insert the link into the frontier (we can assume that hash is correct)
        if ( !visited->contains( message ) )
            {
            visited->insert( message );
            Link link( message );
            frontier->PushUrl( link );
            }
        }
    catch ( String e )
        {
        Print( String("Exception: ") + e, threadID );
        }
    catch ( ... )
        {
        Print("Exception: uncaught", threadID);
        }

    if ( task.deleteArgs )
        delete fd;
    }

String ConnectHandler::handleConnect( int fd, size_t threadID )
    {
    char msg[ MAX_MESSAGE_SIZE + 1 ];
    memset( msg, 0, MAX_MESSAGE_SIZE + 1 );
    size_t cumsum = 0;
    ssize_t bytes = 0;
	do {
		bytes = recv(fd, msg + cumsum, MAX_MESSAGE_SIZE - cumsum, 0);
		if (bytes == -1)
            {
            close( fd );
			throw String("Problem reading byte stream");
            }
		cumsum += bytes;
	} while ( bytes > 0 );

    close( fd );
    if ( cumsum == 0 )
        throw String("Empty message rec");

    return String( msg, cumsum );
    }

void ConnectHandler::incrementCountURL( size_t threadID )
    {
    CriticalSection s(&countURLmutex);
    if ( ++countURL % 100 == 0 )
        {
        Print("100 more URLs received", threadID ); 
        countURL = 0;
        }
    }

// ----- Send Manager

SendManager::SendManager( Init init, Frontier *frontier, FileBloomfilter *visited )
        : CrawlerManager( init, frontier, visited )
        {
        for ( size_t i = 0; i < NUM_MACHINES; ++i )
            {
            failedMachine[i] = 0;
            }
        MutexInit(&failedMachineMutex, nullptr);
        }

void SendManager::DoTask( Task task, size_t threadID )
    {
    Link *link = ( Link * ) task.args;
    //assert( link->URL[ link->URL.size( ) - 1 ] != '\n' );
    
    size_t mID = fnvHash( link->URL.cstr( ), link->URL.size( ) ) % NUM_MACHINES;
    
    String output = "URL sent (";
    output += ltos( myID );
    output += "->";
    output += ltos( mID );
    output += "): ";
    output += link->URL;
    if ( mID == myID )
        {
        if ( !this->visited->contains( link->URL ) )
            {
            this->visited->insert( link->URL );
            this->frontier->PushUrl( *link );
            }
        //Print( output, threadID );
        }
    else
        {
        try 
            {
            sendURL( link->URL, mID );
            //Print( output, threadID );
            failedMachine[ mID ] = 0;
            }
        catch ( String e )
            {
            //Print( e, threadID ); 
            incrementCountFailMachine( mID, threadID );
            }
        catch ( ... )
            {
            Print( "Uncaught exception", threadID );
            }
        }

    if ( task.deleteArgs )
        delete link;
    }

void SendManager::incrementCountFailMachine( size_t machineID, size_t threadID )
    {
    CriticalSection s(&failedMachineMutex);
    if ( ++failedMachine[ machineID ] % 5 == 0 )
        {
        Print(String("Unable to connect to machine: ") + ltos(machineID), threadID);
        failedMachine[ machineID ] = 0;
        }
    }

void SendManager::sendURL( String url, size_t machineID )
    {
    int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sockfd == -1 )
        throw String("Unable to open stream socket");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    int result = inet_pton( AF_INET, HOST[ machineID ] , &addr.sin_addr );
    if ( result == -1 )
        throw String("Error creating IP address: invalid family");
    else if ( result == 0 )
        throw String("Error creating IP address: invalid cstr");
	addr.sin_port = htons( PORT );

    String address( HOST[ machineID ] );
    address += String(":") + ltos( PORT );
    String machine = ltos(machineID) + String(" (") + address + String(")");

    if ( connect( sockfd, ( sockaddr * ) &addr, sizeof( addr ) ) == -1)
        {
        close( sockfd );
        throw String("Connection failed to machine: ") + machine;
        }

    int bytes = send( sockfd, url.cstr(), url.size( ), 0 );
    if ( bytes == -1 )
        {
        close( sockfd );
        throw String("Sending bytes failed to machine: ") + machine;
        }
    close( sockfd );
    }