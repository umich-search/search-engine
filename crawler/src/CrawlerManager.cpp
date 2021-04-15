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

// ListenManager::ListenManager( Init init, Frontier *frontier, FileBloomfilter *visited, int port )   
//     : CrawlerManager( init, frontier, visited ), listenPort( port ), submitPtr( new pthread_t )
//     {
//     int socketFD = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

//     int yesval = 1;
//     setsockopt( socketFD, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof( yesval ) );

//     struct sockaddr_in addr;
//     makeListenAddr( &addr, port );
//     bind( socketFD, ( sockaddr * ) &addr, sizeof( addr ) );
//     // port = getPort( socketFD );

//     listen( socketFD, queue_size );
//     pthread_create( submitPtr, nullptr, submitThread, this );

//     std::cout << "Listenning thread created! socketfd = " << socketFD << std::endl;
//     std::cout << "Listening on host " << inet_ntoa( addr.sin_addr ) << ":" << ntohs( addr.sin_port ) << std::endl;

//     // pthread_detach( *submitPtr );

//     // std::cout << "Listenning thread detahced\n";
//     }

// ListenManager::~ListenManager( )
//     {
//     delete submitPtr;
//     close( socketFD );
//     }

// void ListenManager::submitConnection( )
//     {
//     while ( true )
//         {
//         Print( String( "Waiting for connection" ), 0 );
//         int connectionfd = accept( socketFD, 0, 0 );
//         Print( String( "connection accepted!" ), 0 );
//         if ( connectionfd < 0 )
//             {
//             throw "Cannot setup connections";
//             }
//         this->PushTask( ( void * )( new int ( connectionfd ) ), true );
//         Print( String( "Connectionfd" ) + ltos( connectionfd ) + String( "was pushed to task queue "), 233 );
//         }
//     }

// void ListenManager::DoTask( Task task, size_t threadID )
//     {
//     int connectionfd = *( ( int * )( task.args ) );
//     Print(String("Listen: connection: ") + ltos(connectionfd), threadID);
//     char msg[ MAX_MESSAGE_SIZE + 1 ];
//     memset( msg, 0, sizeof( msg ) );

//     // Call recv() enough times to consume all the data the client sends.
//     size_t recvd = 0;
//     ssize_t rval;
//     do 
//         {
//         // Receive as many additional bytes as we can in one call to recv()
//         // (while not exceeding MAX_MESSAGE_SIZE bytes in total).
//         rval = recv( connectionfd, msg + recvd, MAX_MESSAGE_SIZE - recvd, 0 );
//         assert ( rval > -1 );
//         recvd += rval;
//         } 
//     while ( rval > 0 );  // recv() returns 0 when client closes
//     close( connectionfd );

//     String pt = "Listen: Message received = ";
//     pt += String( msg );
//     Print( pt, threadID );

//     // Verify URL hash
//     uint32_t hash = fnvHash( ( const char * )msg, recvd );
//     if ( hash % numMachine == this->myIndex ) 
//         {
//         if ( !this->visited->contains( msg ) )
//             {
//             // Push the URL to the frontier
//             this->visited->insert( msg );
//             Link lk ( msg );
//             this->frontier->PushUrl( lk );
//             }
//         }
//     else
//         std::cerr << "Sent to the wrong machine: " << this->myIndex << " where it supposed to be " << hash % numMachine << std::endl; 
//     }


ListenManager::ListenManager( Init init, Frontier *frontier, FileBloomfilter *visited, int port )
    : CrawlerManager( init, frontier, visited ), listenPort( port ), threadPtr( new pthread_t )
    {
    pthread_create( threadPtr, nullptr, listenThread, ( void * )this );
    pthread_detach( *threadPtr );
    }

ListenManager::~ListenManager( )
    {
    delete threadPtr;
    }

int ListenManager::startServer( )
    {
    // (1) Create socket
	int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd == -1 ) 
        {
		std::cerr << "Error opening stream socket\n";
		return -1;
	    }

	// (2) Set the "reuse port" socket option
	int yesval = 1;
	if ( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof( yesval ) ) == -1) 
        {
		std::cerr << "Error setting socket options\n";
		return -1;
	    }

	// (3) Create a sockaddr_in struct for the proper port and bind() to it.
	struct sockaddr_in addr;
	if ( makeListenAddr( &addr, listenPort ) == -1 ) 
        {
		return -1;
	    }

	// (3b) Bind to the port.
	if ( bind( sockfd, ( sockaddr * ) &addr, sizeof( addr ) ) == -1 ) 
        {
		std::cerr << "Error binding stream socket\n";
		return -1;
	    }

	// (3c) Detect which port was chosen.
	int port = getPort( sockfd );
	printf( "Server listening on port %d...\n", listenPort );

	// (4) Begin listening for incoming connections.
	listen( sockfd, queue_size );

	// (5) Serve incoming connections one by one forever.
	while ( true ) 
        {
        int connectionfd = accept( sockfd, 0, 0 );
		if ( connectionfd == -1 ) 
            {
			std::cerr << "Error accepting connection\n";
			return -1;
            }

		if ( handleConnect( connectionfd ) == -1)
			return -1;
        }
    }

int ListenManager::handleConnect( int fd )
    {
    std::cout << "Connection received!\n";
    char msg[ MAX_MESSAGE_SIZE + 1 ];
    memset( msg, 0, MAX_MESSAGE_SIZE + 1 );

    size_t cumsum = 0;
    ssize_t bytes;
    while ( ( bytes = read( fd, msg + cumsum, MAX_MESSAGE_SIZE - cumsum ) ) > 0 )
        {
        
        }
    
    }

SendManager::SendManager( Init init, Frontier *frontier, FileBloomfilter *visited, int port )
        : CrawlerManager( init, frontier, visited ), sendPort( port )
        {
        std::cout << "Send manager send to port " << sendPort << std::endl;
        }

void SendManager::DoTask( Task task, size_t threadID )
    {
    Link *link = ( Link * ) task.args;
    assert( link->URL[ link->URL.size( ) - 1 ] != '\n' );
    
    size_t mID = fnvHash( link->URL.cstr( ), link->URL.size( ) ) % numMachine;
    
    if ( mID == myIndex )
        {
        if ( !this->visited->contains( link->URL ) )
            {
            this->visited->insert( link->URL );
            this->frontier->PushUrl( *link );

            String pt = "Send: My own url = ";
            pt += link->URL;
            Print( pt + ltos( pt.size( ) ), threadID );
            }
        }
    else
        {
        String pt = "Send: other's url = ";
        pt += link->URL;
        Print( pt, threadID );

        int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
        struct sockaddr_in addr;
        makeSendAddr( &addr, Host[ mID ], sendPort );
        Print(String("Send: send to address: ") + String(inet_ntoa(addr.sin_addr)) + String( ":" ) 
            + ltos( (int)ntohs( addr.sin_port ) ), threadID );
        int result = connect( sockfd, ( sockaddr * ) &addr, sizeof( addr ) );
        Print(String("Send: connect result: ") + ltos(result), threadID);
        result = send( sockfd, link->URL.cstr( ), link->URL.size( ), 0 );
        Print(String("Send: sent bytes: ") + ltos(result), threadID);
        close( sockfd );
        }

    if ( task.deleteArgs )
        delete link;
    }

void makeSendAddr( struct sockaddr_in *addr, const char *hostname, int port ) 
    {
	addr->sin_family = AF_INET;
	struct hostent *host = gethostbyname( hostname );
	if ( !host )
		std::cerr << "Unknown host.\n";
	memcpy( &( addr->sin_addr ), host->h_addr, host->h_length );
	addr->sin_port = htons( port );
    }

int makeListenAddr(struct sockaddr_in *addr, int port) 
    {
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_port = htons(port);
    return 0;
    }

 int getPort( int sockfd ) {
 	struct sockaddr_in addr;
	socklen_t length = sizeof( addr );
	if ( getsockname( sockfd, ( sockaddr * ) &addr, &length ) == -1 ) 
        {
		std::cerr << "Error getting the socket address\n";
		return -1;
	    }
	// network byte order to host byte order.
	return ntohs( addr.sin_port );
 }