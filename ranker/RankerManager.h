// ranker manager = query server

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
#include "CrawlerManager.h"  // import host names

#define MAX_MESSAGE_SIZE 512

// query server, handles and manages input queries and ranking results
class QueryServer
    {
    private:
        int sockfd, port;  // network interface to rank servers
        size_t queueSize;

        ::vector< url_score > mergedScores;
        const size_t mergeSize;  // define the maximum size of the mergedScores


        // ********* Helper Methods ************* //

        // construct url_score from string
        ::vector< url_score > deserializeScores( String& urlStr )
            {
            ::vector< url_score > res;
            size_t iter = 0, start = iter;
            const char urlTag = '$', titleTag = '#', scoreTag = '@';
            int score, completedSecNum = 0;  // when finish = 3
            String URL, title;
            for ( ; iter < urlStr.size( ); ++iter )
                {
                if ( urlStr[ iter ] == urlTag )
                    {
                    URL = String( urlStr.cstr( ) + start, iter - start );
                    start = iter + 1;
                    if ( completedSecNum != 0 )
                        throw "Malformed rank server response1\n";
                    
                    ++completedSecNum;
                    }
                else if ( urlStr[ iter ] == titleTag )
                    {
                    title = String( urlStr.cstr( ) + start, iter - start );
                    start = iter + 1;
                    if ( completedSecNum != 1 )
                        throw "Malformed rank server response1\n";
                        
                    ++completedSecNum;
                    }
                else if ( urlStr[ iter ] == scoreTag )
                    {
                    score = atoi( String( urlStr.cstr( ) + start, iter - start ).cstr( ) );
                    start = iter + 1;
                    if ( completedSecNum != 2 )
                        throw "Malformed rank server response1\n";
                        
                    completedSecNum = 0;
                    url_score tmp( URL, title, score );
                    res.pushBack( tmp );
                    }
                else
                    continue;
                }
            return res;
            }
       
        // merge two sorted arrays of url_score's
        ::vector< url_score > merge( ::vector< url_score >& x, ::vector< url_score >& y )
            {
            ::vector< url_score > tmp;
            size_t xi = 0, yi = 0;
            while ( xi < x.size( ) && yi < y.size( ) 
                && tmp.size( ) < mergeSize )
                {
                if ( x[ xi ].score < y[ yi ].score )
                    {
                    tmp.pushBack( y[ yi ] );
                    ++yi;
                    }
                else
                    {
                    tmp.pushBack( x[ xi ] );
                    ++xi;
                    }
                }

            // mergeScores is full
            if ( tmp.size( ) == mergeSize )
                return tmp;
            
            // y exhasts
            if ( xi < x.size( ) )
                {
                while ( xi < x.size( ) && tmp.size( ) < mergeSize )
                    tmp.pushBack( x[ xi++ ] );
                }

            // x exhausts
            if ( yi < y.size( ) )
                {
                while ( yi < y.size( ) && tmp.size( ) < mergeSize )
                    tmp.pushBack( y[ yi++ ] );
                }

            return tmp;
            }


        // ********* Communication Methods ************* //

        // send the query to every distributed rank server
        int sendQuery( String& query )
            {
            for ( size_t i = 0; i < NUM_MACHINES; ++i )
                {
                 int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
                if ( sockfd == -1 )
                    {
                    std::cerr << "Unable to open stream socket";
                    return -1;
                    }

                struct sockaddr_in addr;
                addr.sin_family = AF_INET;
                int result = inet_pton( AF_INET, HOST[ i ] , &addr.sin_addr );
                if ( result == -1 )
                    { 
                    std::cerr << "Error creating IP address: invalid family";
                    return -1;
                    }
                else if ( result == 0 )
                    {
                    std::cerr << "Error creating IP address: invalid cstr";
                    return -1;
                    }
                addr.sin_port = htons( PORT );

                String address( HOST[ i ] );
                address += String(":") + ltos( PORT );
                String machine = ltos( i ) + String(" (") + address + String(")");

                if ( connect( sockfd, ( sockaddr * ) &addr, sizeof( addr ) ) == -1)
                    {
                    close( sockfd );
                    std::cerr << "Connection failed to machine: " <<  machine;
                    return -1;
                    }

                int bytes = send( sockfd, query.cstr(), query.size( ), 0 );
                if ( bytes == -1 )
                    {
                    close( sockfd );
                    std::cerr << "Sending bytes failed to machine: " <<  machine;
                    return -1;
                    }
                close( sockfd );
                }
            return 0;
            }

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
            // (4) Begin listening for incoming connections.
            if ( listen( sockfd, queueSize ) == -1 )
                throw String("Socket listen failed");
            
            String output = "Listening on port ";
            output += ltos( port );

            std::cout << output << std::endl;
            }

        // collect the results from all distributed rank servers
        void collectResult( )
            {
            // (5) Serve incoming connections one by one forever.
            for ( int i = 0; i < NUM_MACHINES; ++i  )
                {
                int connectionfd = accept( sockfd, 0, 0 );
                if ( connectionfd == -1 ) 
                    throw String("Unable to accept connection");
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

        void handleConnect( int connectionfd )
            {
            char msg[ MAX_MESSAGE_SIZE + 1 ];
            memset( msg, 0, MAX_MESSAGE_SIZE + 1 );
            size_t cumsum = 0;
            ssize_t bytes = 0;
            do {
                bytes = recv( connectionfd, msg + cumsum, MAX_MESSAGE_SIZE - cumsum, 0);
                if (bytes == -1)
                    {
                    close( connectionfd );
                    throw String("Problem reading byte stream");
                    }
                cumsum += bytes;
            } while ( bytes > 0 );

            close( connectionfd );
            if ( cumsum == 0 )
                throw String("Empty message rec");
            msg[ cumsum ] = 0;
            String urlStr( msg );

            ::vector< url_score > scores = deserializeScores( urlStr );
            mergedScores = merge( mergedScores, scores );
            }

    public:
        QueryServer( );
        ~QueryServer( );

        // decode the received message to obtain the query string
        // called by the main server
        String deserializeQueryMsg( const char *msg );

        // main server calls this function when it receives a query from the user
        // call deserializeQueryMsg before calling this function
        ::vector< url_score > CollectRanks( String& query )
            {
            if ( mergedScores.size( ) > 0 )
                mergedScores.resize( 0 );  // clear buffer

            // TODO
            startServer( );
            if ( sendQuery( query ) == -1 )
                throw "Error sending query to machines!";
            collectResult( );

            return mergedScores;
            }

    };

