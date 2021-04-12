#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include "CrawlerManager.h"
#include "Common.h"
#include "HtmlParser.h"

#define MAX_MESSAGE_SIZE 512

void makeSendAddr(struct sockaddr_in *addr, const char *hostname, int port);
void makeListenAddr(struct sockaddr_in *addr, int port);
int getPort( int sockFd );

ListenManager::ListenManager( Init init, Frontier *frontier, FileBloomfilter *visited, size_t machineID )   
    : CrawlerManager( init, frontier, visited, machineID )
    {
    int socketFD = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    int yesval = 1;
    setsockopt( socketFD, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof( yesval ) );

    struct sockaddr_in addr;
    makeListenAddr( &addr, port );
    bind( socketFD, ( sockaddr * ) &addr, sizeof( addr ) );
    // port = getPort( socketFD );

    listen( socketFD, queue_size );
    pthread_t submit;
    pthread_create( &submit, nullptr, submitThread, this );
    pthread_detach( submit );
    }

ListenManager::~ListenManager( )
    {
    close( socketFD );
    }

void ListenManager::submitConnection( )
    {
    while ( this->alive )
        {
        int connectionfd = accept( socketFD, 0, 0 );
        if ( connectionfd < 0 )
            {
            throw "Cannot setup connections";
            }
        this->PushTask( ( void * )( new int ( connectionfd ) ), true );
        }
    }

void ListenManager::DoTask( Task task, size_t threadID )
    {
    int connectionfd = *( ( int * )( task.args ) );
    char msg[ MAX_MESSAGE_SIZE + 1 ];
    memset( msg, 0, sizeof( msg ) );

    // Call recv() enough times to consume all the data the client sends.
    size_t recvd = 0;
    ssize_t rval;
    do 
        {
        // Receive as many additional bytes as we can in one call to recv()
        // (while not exceeding MAX_MESSAGE_SIZE bytes in total).
        rval = recv( connectionfd, msg + recvd, MAX_MESSAGE_SIZE - recvd, 0 );
        assert ( rval > -1 );
        recvd += rval;
        } 
    while ( rval > 0 );  // recv() returns 0 when client closes
    close( connectionfd );

    // Verify URL hash
    uint32_t hash = fnvHash( ( const char * )msg, recvd );
    if ( hash % numMachine == this->myIndex ) 
        {
        // Push the URL to the frontier
        Link lk ( msg );
        this->frontier->PushUrl( lk );
        }
    else
        std::cerr << "Sent to the wrong machine: " << this->myIndex << " where it supposed to be " << hash % numMachine << std::endl; 
    }

void SendManager::DoTask( Task task, size_t threadID )
    {
    Link *link = ( Link * ) task.args;
    
    size_t mID = fnvHash( link->URL.cstr( ), link->URL.size( ) ) % numMachine;
    
    if ( mID == myIndex )
        this->frontier->PushUrl( *link );
    else
        {
        int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
        struct sockaddr_in addr;
        makeSendAddr( &addr, Host[ mID ], port );
        connect( sockfd, ( sockaddr * ) &addr, sizeof( addr ) );
        send( sockfd, link->URL.cstr( ), link->URL.size( ), 0 );
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

void makeListenAddr(struct sockaddr_in *addr, int port) 
    {
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_port = htons(port);
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