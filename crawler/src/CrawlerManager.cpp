#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include "CrawlerManager.h"
#include "Common.h"
#define MAX_MESSAGE_SIZE 512
void ListenManager::DoTask( Task task, size_t threadID )
    {
    while ( alive )
        {
        // Listen on a socket for incoming links
        // Create a TCP/IP socket.
        int socketFD = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
        // (2) Set the "reuse port" socket option
	    int yesval = 1;
	    if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof(yesval)) == -1) {
            perror("Error setting socket options");
            return -1;
	    }
        // (3) Create a sockaddr_in struct for the proper port and bind() to it.
        struct sockaddr_in addr;
        if (make_server_sockaddr(&addr, port) == -1) {
            return -1;
        }
        // (3b) Bind to the port.
        if (bind(socketFD, (sockaddr *) &addr, sizeof(addr)) == -1) {
            perror("Error binding stream socket");
            return -1;
        }

        // (3c) Detect which port was chosen.
	    port = get_port_number(socketFD);

        // (4) Begin listening for incoming connections.
	    listen(socketFD, queue_size);

        while ( true )
            {
            int connectionfd = accept(socketFD, 0, 0);
		    assert (connectionfd > -1);
            // (1) Receive message from client.

            char msg[MAX_MESSAGE_SIZE + 1];
            memset(msg, 0, sizeof(msg));

            // Call recv() enough times to consume all the data the client sends.
            size_t recvd = 0;
            ssize_t rval;
            do {
                // Receive as many additional bytes as we can in one call to recv()
                // (while not exceeding MAX_MESSAGE_SIZE bytes in total).
                rval = recv(connectionfd, msg + recvd, MAX_MESSAGE_SIZE - recvd, 0);
                assert ( rval > -1 );
                recvd += rval;
            } while (rval > 0);  // recv() returns 0 when client closes

            // (2) Print out the message
            printf("Client %d says '%s'\n", connectionfd, msg);

            // (4) Close connection
            close(connectionfd);

            // Verify URL hash
            uint32_t hash = fnvHash((const char *)msg, recvd);
            if ( hash % numMachine == this->myIndex ) 
                {
                // Push the URL to the frontier
                this->frontier->PushUrl(Link(msg));
                }
            }
        
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