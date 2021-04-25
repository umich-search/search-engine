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
#include "query_Compiler.h"
#include "Results.h"

#define MAX_MESSAGE_SIZE 512

class RankServer
    {
    private:
        // communication attributes
        int sockfd, port, mPort;  // listen socket fd and port, manager port
        size_t queueSize;  // the size of the listen queue
        std::string managerIP;  // the ip address of the manager

        // ranking attributes
        Dictionary dict;
        Ranker rk;

        // serialize the vector of scores into network message
        std::string serializeScores( ::vector< url_score >& scores )
            {
            std::string msg;
            for ( size_t i = 0; i < scores.size( ); ++i )
                {
                msg += serializeUrlScore( &scores[ i ] );
                // delete the heap memory
                // delete scores[ i ];
                // scores[ i ] = nullptr;
                }
            return msg;
            }

        // build the query compiler and the constraint solver 
        // call the ranker to obtain results
        std::string retrieveDocRank( char *query )
            {
            Dictionary d( 0 );
            ::vector< url_score > scores = Results( &d, query );
            printRanks( scores );
            return serializeScores( scores );
            }

        // set up socket parameters
        void startServer( )
            {
            // (1) Create socket
            sockfd = socket( AF_INET, SOCK_STREAM, 0 );
            if ( sockfd == -1 ) 
                throw std::string("Unable to open stream socket");

            // (2) Set the "reuse port" socket option
            int yesval = 1;
            if ( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof( yesval ) ) == -1) 
                throw std::string("Unable to set socket options");

            // (3) Create a sockaddr_in struct for the proper port and bind() to it.
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = INADDR_ANY;
            addr.sin_port = htons( port );

            // (3b) Bind to the port.
            if ( bind( sockfd, ( sockaddr * ) &addr, sizeof( addr ) ) == -1 ) 
                throw std::string("Unable to bind stream socket");

            // (3c) Detect which port was chosen.
            socklen_t length = sizeof( addr );
            if ( getsockname( sockfd, ( sockaddr * ) &addr, &length ) == -1 ) 
                throw std::string("Unable to detect port");
            if ( ntohs( addr.sin_port ) != port )
                throw std::string("Incorrect listen port");
            }

        // start the server to listen traffics
        void runServer( )
            {
            // (4) Begin listening for incoming connections.
            if ( listen( sockfd, queueSize ) == -1 )
                throw std::string("Socket listen failed");
            
            std::cout << "Ranker Server Listening on port " << port << std::endl;

            // (5) Serve incoming connections one by one forever.
            while ( true ) 
                {
                std::cout << "waiting to accept\n";
                int connectionfd = accept( sockfd, 0, 0 );
                std::cout << "accepted\n";
                if ( connectionfd == -1 ) 
                    throw std::string("Unable to accept connection");
                try
                    {
                    // Add the connection to be handled
                    handleConnect( connectionfd );
                    }
                catch( const char * e )
                    {
                    std::cerr << e << std::endl;
                    }
                }

            close( sockfd );
            }
    
        // handle an established connection:
        // retrieve document ranks and send to the manager
        int handleConnect( int connectFd )
            {
            char msg[ MAX_MESSAGE_SIZE + 1 ];
            memset( msg, 0, MAX_MESSAGE_SIZE + 1 );
            size_t cumsum = 0;
            ssize_t bytes = 0;
            do {
                bytes = recv( connectFd, msg + cumsum, MAX_MESSAGE_SIZE - cumsum, 0 );
                if (bytes == -1)
                    {
                    close( connectFd );
                    std::cerr << "Problem reading byte stream with errno = " << strerror( errno ) << std::endl;
                    return -1;
                    }
                cumsum += bytes;
            } while ( bytes > 0 );

            close( connectFd );
            if ( cumsum == 0 )
                {
                std::cerr << "Empty message rec";
                return -1;
                }
            msg[ cumsum ] = 0;

            // send message to the manager
            
            // TODO: change docRank to the ranker result
            std::string docRank = retrieveDocRank( msg );
            // if ( docRank.empty( ) )
            //     docRank = "$#0@";
            // std::string docRank = "google.com$GOOGLE#4@";
            std::cout << "message and returns: " << msg << "; " << docRank << std::endl;
            if ( sendMessage( docRank ) == -1 )
                {
                std::cerr << "Unable to send messages to the manager\n";
                return -1;
                }
            return 0;
            }

        // send the ranked result to the manager
        int sendMessage( std::string& msg )
            {
            int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
            if ( sockfd == -1 )
                throw std::string("Unable to open stream socket");

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            inet_pton( AF_INET, managerIP.c_str( ), &addr.sin_addr );
            addr.sin_port = htons( mPort );

            std::cout << "trying to connect\n";
            if ( connect( sockfd, ( sockaddr * ) &addr, sizeof( addr ) ) == -1)
                {
                close( sockfd );
                std::cerr << "Unable to connect to the ranker manager\n";
                return -1;
                }
            std::cout << "connection established\n";

            int bytes = send( sockfd, msg.c_str(), msg.size( ), 0 );
            if ( bytes == -1 )
                {
                close( sockfd );
                std::cerr << "Unable to send the message\n";
                return -1;
                }

            std::cout << "message sent to the manager: \n" << inet_ntoa( addr.sin_addr ) << ":" << ntohs( addr.sin_port ) << std::endl;

            close( sockfd );
            return 0;
            }
    
    public:
        RankServer( std::string mIP, int port_, int mPort_, size_t qSize = 1024 ) 
            : port( port_ ), mPort( mPort_ ), managerIP( mIP ), 
            queueSize( qSize ), dict( 0 )
            {
            }
        
        ~RankServer( )
            {
            }

        void Start( )
            {
            try
                {
                startServer( );
                runServer( );
                }
            catch( const char *e )
                {
                std::cerr << e << std::endl;
                }
            }
    };


