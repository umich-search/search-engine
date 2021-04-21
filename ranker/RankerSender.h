#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "ranker.h"
#include "mString.h"
#include "Vector.h"

#define MAX_MESSAGE_SIZE 512

class RankServer
    {
    private:
        int sockfd, port, mPort;  // listen socket fd and port, manager port
        size_t queueSize;  // the size of the listen queue
        String managerIP;  // the ip address of the manager

        // serialize the vector of scores into network message
        String serializeScores( ::vector< url_score * >& scores );

        // build the query compiler and the constraint solver 
        // call the ranker to obtain results
        String retrieveDocRank( String& query );

        // decode the received message to obtain the query string
        String deserializeQueryMsg( const char *msg );

        // set up socket parameters
        void startServer( )
            {
            // (1) Create socket
            sockfd = socket( AF_INET, SOCK_STREAM, 0 );
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
            addr.sin_port = htons( port );

            // (3b) Bind to the port.
            if ( bind( sockfd, ( sockaddr * ) &addr, sizeof( addr ) ) == -1 ) 
                throw String("Unable to bind stream socket");

            // (3c) Detect which port was chosen.
            socklen_t length = sizeof( addr );
            if ( getsockname( sockfd, ( sockaddr * ) &addr, &length ) == -1 ) 
                throw String("Unable to detect port");
            if ( ntohs( addr.sin_port ) != port )
                throw String("Incorrect listen port");
            }

        // start the server to listen traffics
        void runServer( )
            {
            // (4) Begin listening for incoming connections.
            if ( listen( sockfd, queueSize ) == -1 )
                throw String("Socket listen failed");
            
            String output = "Listening on port ";
            output += ltos( port );

            // (5) Serve incoming connections one by one forever.
            while ( true ) 
                {
                int connectionfd = accept( sockfd, 0, 0 );
                if ( connectionfd == -1 ) 
                    throw String("Unable to accept connection");
                // Add the connection to be handled
                handleConnect( connectionfd );
                }

            close( sockfd );
            }
    
        // handle an established connection with the manager
        int handleConnect( int connectFd )
            {
            char msg[ MAX_MESSAGE_SIZE + 1 ];
            memset( msg, 0, MAX_MESSAGE_SIZE + 1 );
            size_t cumsum = 0;
            ssize_t bytes = 0;
            do {
                bytes = recv( sockfd, msg + cumsum, MAX_MESSAGE_SIZE - cumsum, 0 );
                if (bytes == -1)
                    {
                    close( sockfd );
                    std::cerr << "Problem reading byte stream";
                    return -1;
                    }
                cumsum += bytes;
            } while ( bytes > 0 );

            close( sockfd );
            if ( cumsum == 0 )
                {
                std::cerr << "Empty message rec";
                return -1;
                }
            msg[ cumsum ] = 0;

            // send message to the manager
            String query, docRank;
            query = deserializeQueryMsg( msg );
            docRank = retrieveDocRank( query );
            if ( sendMessage( docRank ) == -1 )
                {
                std::cerr << "Unable to send messages to the manager\n";
                return -1;
                }
            return 0;
            }

        // send the ranked result to the manager
        int sendMessage( String& msg )
            {
            int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
            if ( sockfd == -1 )
                throw String("Unable to open stream socket");

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            int result = inet_pton( AF_INET, managerIP.cstr( ), &addr.sin_addr );
            if ( result == -1 )
                throw String("Error creating IP address: invalid family");
            else if ( result == 0 )
                throw String("Error creating IP address: invalid cstr");
            addr.sin_port = htons( mPort );

            String address( managerIP );
            address += String(":") + ltos( mPort );
            // String machine = ltos(machineID) + String(" (") + address + String(")");

            if ( connect( sockfd, ( sockaddr * ) &addr, sizeof( addr ) ) == -1)
                {
                close( sockfd );
                std::cerr << "Unable to connect to the ranker manager\n";
                return -1;
                }

            int bytes = send( sockfd, msg.cstr(), msg.size( ), 0 );
            if ( bytes == -1 )
                {
                close( sockfd );
                std::cerr << "Unable to send the message\n";
                return -1;
                }
            close( sockfd );
            return 0;
            }
    
    public:
        RankServer( int port_, int mPort_, size_t qSize = 1024 ) 
            : port( port_ ), mPort( mPort_ ), queueSize( qSize ) 
            {
            }
        
        ~RankServer( )
            {
            }

        void Start( )
            {
            startServer( );
            runServer( );
            }
    };


