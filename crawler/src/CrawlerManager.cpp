#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include "CrawlerManager.h"
#include "Common.h"

void ListenManager::DoTask( Task task, size_t threadID )
    {
    while ( alive )
        {
        // Listen on a socket for incoming links

        // Verify URL hash

        // Push the URL to the frontier

        }
    }

void SendManager::DoTask( Task task, size_t threadID )
    {
    Link *link = (Link *) task.args;
    
    size_t mID = fnvHash( link->URL.cstr( ), link->URL.size( ) ) % NUM_MACHINE;
    
    // (1) Open a socket
    int sockfd = socket( AF_INET, SOCK_STREAM, 0 );

	// (2) Create a sockaddr_in to specify remote host and port
	struct sockaddr_in addr;
	if ( make_client_sockaddr( &addr, HOST[ mID ], PORT ) == -1 ) 
		std::cerr << "Error make client address!\n";

	// (3) Connect to remote server
	if ( connect( sockfd, ( sockaddr * ) &addr, sizeof( addr ) ) == -1 ) 
		std::cerr << "Error connecting stream socket\n";
	
	// (4) Send message to remote server
	if ( send( sockfd, message, strlen( message ), 0 ) == -1 ) 
		std::cerr << "Error sending on stream socket\n";

	// (5) Close connection
	close( sockfd );

    // Send the link to the appropriate machine
    
    // machine ID = hash(URL) % N machines
    
    // If self, push to frontier
    
    if ( task.deleteArgs )
        delete link;
    }