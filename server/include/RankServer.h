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
        int port;
        size_t queueSize;  // the size of the listen queue

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
        std::string retrieveDocRank( const char *query )
            {
            Dictionary d( 0 );
            ::vector< url_score > scores = Results( &d, query );
            printRanks( scores );
            return serializeScores( scores );
            }

        // set up socket parameters
        int startServer( )
            {
            // (1) Create socket
            int sockFD = socket( AF_INET, SOCK_STREAM, 0 );
            if ( sockFD == -1 ) 
                throw std::string("Unable to open stream socket");

            // (2) Set the "reuse port" socket option
            int yesval = 1;
            if ( setsockopt( sockFD, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof( yesval ) ) == -1) 
                throw std::string("Unable to set socket options");

            // (3) Create a sockaddr_in struct for the proper port and bind() to it.
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = INADDR_ANY;
            addr.sin_port = htons( port );

            // (3b) Bind to the port.
            if ( bind( sockFD, ( sockaddr * ) &addr, sizeof( addr ) ) == -1 ) 
                throw std::string("Unable to bind stream socket");

            // (3c) Detect which port was chosen.
            socklen_t length = sizeof( addr );
            if ( getsockname( sockFD, ( sockaddr * ) &addr, &length ) == -1 ) 
                throw std::string("Unable to detect port");
            if ( ntohs( addr.sin_port ) != port )
                throw std::string("Incorrect listen port");
            
            return sockFD;
            }

        // start the server to listen traffics
        void runServer( int sockFD )
            {
            // (4) Begin listening for incoming connections.
            if ( listen( sockFD, queueSize ) == -1 )
                throw std::string("Socket listen failed");
            
            std::cout << "Rank Server listening on port: " << port << std::endl;

            // (5) Serve incoming connections one by one forever.
            while ( true ) 
                {
                int connectFD = accept( sockFD, 0, 0 );
                if ( connectFD == -1 ) 
                    throw std::string("Unable to accept connection");
                try
                    {
                    std::cout << "Connection received: " << connectFD << std::endl;
                    std::string query = handleConnect( connectFD );
                    std::cout << "Received query: " << query << std::endl;
                    // TODO: change docRank to the ranker result
                    std::string docRank = retrieveDocRank( query.c_str() );
                    //std::string docRank = "https://google.com$GOOGLE#4@";
                    std::cout << "query: " << query << " result: " << docRank << std::endl;
                    sendResponse( connectFD, docRank );
                    }
                catch( std::string e )
                    {
                    std::cerr << e << std::endl;
                    }
                close( connectFD );
                }
            }
    
        // handle an established connection:
        // retrieve document ranks and send to the manager
        std::string handleConnect( int connectFD )
            {
            char msg[ MAX_MESSAGE_SIZE + 1 ];
            memset( msg, 0, MAX_MESSAGE_SIZE + 1 );
            size_t cumsum = 0;
            ssize_t bytes = 0;
            do {
                bytes = recv( connectFD, msg + cumsum, MAX_MESSAGE_SIZE - cumsum, 0 );
                if (bytes == -1)
                    {
                    close( connectFD );
                    throw std::string("Problem reading byte stream with errno = ") + strerror( errno );
                    }          
                cumsum += bytes;
                //std::cout << msg << std::endl;
                if ( msg[ cumsum - 1 ] == '\0' )
                    break;
            } while ( bytes > 0 );

            if ( cumsum == 0 )
                throw std::string("Empty message received");
            
            msg[ cumsum ] = 0;
            return std::string( msg, cumsum );
            }

        // send the ranked result to the manager
        void sendResponse( int connectFD, std::string& msg )
            {
            int bytes = send( connectFD, msg.c_str(), msg.size( ), 0 );
            if ( bytes == -1 )
                throw std::string("Unable to send the message");
            }
    
    public:
        RankServer( int port_, size_t qSize = 1024 ) 
            : port( port_ ), queueSize( qSize ), dict( 0 ), rk()
            {
            }
        
        ~RankServer( )
            {
            }

        void Start( )
            {
            try
                {
                int sockFD = startServer( );
                runServer( sockFD );
                close( sockFD );
                }
            catch( std::string e )
                {
                std::cerr << e << std::endl;
                }
            }
    };


