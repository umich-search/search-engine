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

ListenManager::ListenManager( Init init, Frontier *frontier, FileBloomfilter *visited )
    : CrawlerManager( init, frontier, visited )
    {
    }

ListenManager::~ListenManager( )
    {
    }

void ListenManager::DoLoop( size_t threadID )
    {
    while ( alive )
        {
        try
            {
            int sockfd = startServer( threadID );
            runServer( sockfd, threadID );
            }
        catch ( String e )
            {
            Print( e, threadID );
            }
        catch ( ... )
            {
            Print("Uncaught exception", threadID);
            }
        }
    }

int ListenManager::startServer( size_t threadID )
    {
    // (1) Create socket
	int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd == -1 ) 
        throw String("Error opening stream socket");

	// (2) Set the "reuse port" socket option
	int yesval = 1;
	if ( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof( yesval ) ) == -1) 
        throw String("Error setting socket options");

	// (3) Create a sockaddr_in struct for the proper port and bind() to it.
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons( PORT );

	// (3b) Bind to the port.
	if ( bind( sockfd, ( sockaddr * ) &addr, sizeof( addr ) ) == -1 ) 
        throw String("Error binding stream socket");

	// (3c) Detect which port was chosen.
	socklen_t length = sizeof( addr );
	if ( getsockname( sockfd, ( sockaddr * ) &addr, &length ) == -1 ) 
		throw String("Error detecting port");
    if ( ntohs( addr.sin_port ) != PORT )
        throw String("Error incorrect listen port");
        
    return sockfd;
    }

void ListenManager::runServer( int sockfd, size_t threadID )
    {
	// (4) Begin listening for incoming connections.
	if ( listen( sockfd, QUEUE_SIZE ) == -1 )
        throw String("Error listening on socket");
    
    String output = "Listening on port ";
    output += ltos( PORT );
	Print( output, threadID );

	// (5) Serve incoming connections one by one forever.
	while ( alive ) 
        {
        int connectionfd = accept( sockfd, 0, 0 );
		if ( connectionfd == -1 ) 
            throw String("Error accepting connection");
        try 
            {
            String message = handleConnect( connectionfd, threadID );
            String output = "URL received: ";
            output += message;
            Print( output, threadID );
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
            Print( e, threadID );
            }		
        }
    }

String ListenManager::handleConnect( int fd, size_t threadID )
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
			throw String("Error reading byte stream");
            }
		cumsum += bytes;
	} while ( bytes > 0 );

    close( fd );
    if ( cumsum == 0 )
        throw String("Error empty message");

    return String( msg, cumsum );
    }

// ----- Send Manager

SendManager::SendManager( Init init, Frontier *frontier, FileBloomfilter *visited )
        : CrawlerManager( init, frontier, visited )
        {
        }

void SendManager::DoTask( Task task, size_t threadID )
    {
    Link *link = ( Link * ) task.args;
    assert( link->URL[ link->URL.size( ) - 1 ] != '\n' );
    
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
        Print( output, threadID );
        }
    else
        {
        try 
            {
            sendURL( link->URL, mID, threadID );
            Print( output, threadID );
            }
        catch ( String e )
            {
            Print( e, threadID ); 
            }
        catch ( ... )
            {
            Print( "Uncaught exception", threadID );
            }
        }

    if ( task.deleteArgs )
        delete link;
    }

void SendManager::sendURL( String url, size_t machineID, size_t threadID )
    {
    int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sockfd == -1 )
        throw String("Error opening stream socket");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
	struct hostent *host = gethostbyname( HOST[ machineID ] );
	if ( !host )
        {
        close( sockfd );
		throw String("Error unkown host");
        }

	memcpy( &( addr.sin_addr ), host->h_addr, host->h_length );
	addr.sin_port = htons( PORT );

    String address = String(inet_ntoa(addr.sin_addr));
    address += String(":") + ltos( (int)ntohs( addr.sin_port ) );

    if ( connect( sockfd, ( sockaddr * ) &addr, sizeof( addr ) ) == -1)
        {
        close( sockfd );
        throw String("Error connection failed to address: ") + address;
        }

    int bytes = send( sockfd, url.cstr(), url.size( ), 0 );
    if ( bytes == -1 )
        {
        close( sockfd );
        throw String("Error sending bytes to address: ") + address;
        }
    close( sockfd );
    }