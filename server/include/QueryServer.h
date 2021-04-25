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
#include "CrawlerManager.h"
#include "ThreadPool.h"
#include "Concurrency.h"

#define MAX_MESSAGE_SIZE 512

const size_t NUM_RANKERS = 1;
// const size_t QUEUE_SIZE = 1024;

static const char *RANKER_HOST[ NUM_RANKERS ] = 
    {
        "35.202.123.51",
        // "104.197.37.30",
        // "146.148.73.38",
        // "34.69.231.181",
        // "34.66.107.136",
        // "34.68.201.74",
        // "35.188.164.185",
        // "35.221.27.146",
        // "35.199.41.233",
        // "35.245.62.74",
        // "34.86.92.34",
        // "35.236.254.45",
        // "35.230.186.30",
        // "35.221.49.174",
        
        // "127.0.0.1",
        // "127.0.0.1",
        // "127.0.0.1",
    };

// cloud use
// const int RANKER_PORT = 8888;

const int RANKER_PORT[ ] = 
    {
    8000, 8001, 8002
    };

// struct message 
//     {
//     size_t ID;
//     std::string body;
//     };

// query server, handles and manages input queries and ranking results
class QueryServer : ThreadPool
    {
    private:
        mutex_t mutex;

        int port;  // network interface to rank servers (sockfd for test)
        size_t queueSize;

        ::vector< url_score > mergedScores;
        const size_t mergeSize;  // define the maximum size of the mergedScores

        size_t requestID;
        std::atomic<bool> finished [ NUM_RANKERS ];

        // ********* Helper Methods ************* //

        // construct url_score from string
        ::vector< url_score > deserializeScores( std::string& urlStr )
            {
            ::vector< url_score > res;
            size_t iter = 0, start = iter;
            // const char urlTag = '$', titleTag = '#', scoreTag = '@';
            const char *urlTag = "\r\r\n", *titleTag = "\r\r\r\n", *scoreTag = "\r\r\r\r\n";
            int score, completedSecNum = 0;  // when finish = 3
            std::string URL, title;
            for ( ; iter < urlStr.size( ); )
                {
                if ( !strncmp( urlStr.c_str( ) + iter, urlTag, 3 ) )
                    {
                    if ( completedSecNum != 0 )
                        {
                        std::cerr << "Malformed rank server response1\n";
                        completedSecNum = 0;
                        while( strncmp( urlStr.c_str( ) + iter, scoreTag, 5 ) )
                            ++iter;
                        iter += 5;
                        start = iter;
                        continue;
                        }

                    URL = std::string( urlStr.c_str( ) + start, iter - start );
                    iter += 3;
                    start = iter;
                    
                    ++completedSecNum;
                    }
                else if ( !strncmp( urlStr.c_str( ) + iter, titleTag, 4 ) )
                    {
                    if ( completedSecNum != 1 )
                        {
                        std::cerr << "Malformed rank server response1\n";
                        completedSecNum = 0;
                        while( strncmp( urlStr.c_str( ) + iter, scoreTag, 5 ) )
                            ++iter;
                        iter += 5;
                        start = iter;
                        continue;
                        }
                    title = std::string( urlStr.c_str( ) + start, iter - start );
                    iter += 4;
                    start = iter;
                        
                    ++completedSecNum;
                    }
                else if ( !strncmp( urlStr.c_str( ) + iter, scoreTag, 5 ) )
                    {
                    if ( completedSecNum != 2 )
                        {
                        std::cerr << "Malformed rank server response1\n";
                        completedSecNum = 0;
                        while( strncmp( urlStr.c_str( ) + iter, scoreTag, 5 ) )
                            ++iter;
                        iter += 5;
                        start = iter;
                        continue;
                        }
                    score = atoi( std::string( urlStr.c_str( ) + start, iter - start ).c_str( ) );
                    iter += 5;
                    start = iter;
                    
                    // store the parsed result
                    completedSecNum = 0;
                    url_score tmp( URL, title, score );
                    res.pushBack( tmp );
                    std::cout << "+++++\n" << "deserialized message:\n" << URL << std::endl << title << std::endl << score << "\n++++++++++++++++\n";
                    }
                else
                    {
                    ++iter;
                    }
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
        int sendQuery( size_t machineID, std::string& query )
            {
            std::cout << "Sending to machine: " << machineID << std::endl;
            int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
            if ( sockfd == -1 )
                {
                std::cerr << "Unable to open stream socket";
                return -1;
                }

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            int result = inet_pton( AF_INET, RANKER_HOST[ machineID ] , &addr.sin_addr );
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
            // TODO:change it to 8888
            addr.sin_port = htons( port );
            //addr.sin_port = htons( RANKER_PORT[ machineID ] );

            std::string address( RANKER_HOST[ machineID ] );
            address += std::string(":") + ltos( ntohs( addr.sin_port ) ).cstr( );
            std::string machine = ltos( machineID ).cstr( ) + std::string(" (") + address + std::string(")");

            if ( connect( sockfd, ( sockaddr * ) &addr, sizeof( addr ) ) == -1)
                {
                close( sockfd );
                std::cerr << "Connection failed to machine: " <<  machine << std::endl;
                return -1;
                }
            std::cout << "Connection succeeds to machine " << machine << std::endl;

            // message msg;
            // msg.ID = requestID;
            // msg.body = query;
            int bytes = send( sockfd, query.c_str(), query.size(), 0 );
            if ( bytes == -1 )
                {
                close( sockfd );
                std::cerr << "Sending bytes failed to machine: " <<  machine << std::endl;
                return -1;
                }
            std::cout << query << " sent sucessfully to machine: " << machine << std::endl;
            close( sockfd );
        
            return 0;
            }

        int startServer( )
            {
            // (1) Create socket
            int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
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
            // (4) Begin listening for incoming connections.
            if ( listen( sockfd, queueSize ) == -1 )
                throw std::string("Socket listen failed");
            
            std::cout << "Query Server listening on port: " << ntohs( addr.sin_port ) << std::endl;
            return sockfd;
            }

        // collect the results from all distributed rank servers
        void collectResult( int sockfd )
            {
            // (5) Serve incoming connections one by one forever.
            struct sockaddr_in addr;
            socklen_t addr_len = sizeof( addr );
            int connectionfd = accept( sockfd, (struct sockaddr *)&addr, &addr_len );
            if ( connectionfd == -1 ) 
                throw std::string("Unable to accept connection");
            try
                {
                handleConnect( connectionfd );
                char client[INET_ADDRSTRLEN];
                inet_ntop( AF_INET, (void *)&addr.sin_addr, client, INET_ADDRSTRLEN );
                for ( size_t i = 0; i < NUM_RANKERS; ++i )
                    {
                    if ( strcmp( client, RANKER_HOST[i] ) == 0 )
                        {
                        finished[i] = true;
                        break;
                        }
                    }
                }
                catch( std::string e )
                    {
                    std::cerr << e << std::endl;
                    }
            }

        void handleConnect( int connectionfd )
            {
            char buf[ MAX_MESSAGE_SIZE + 1 ];
            memset( buf, 0, MAX_MESSAGE_SIZE + 1 );
            size_t cumsum = 0;
            ssize_t bytes = 0;
            do {
                bytes = recv( connectionfd, buf + cumsum, MAX_MESSAGE_SIZE - cumsum, 0);
                if (bytes == -1)
                    {
                    close( connectionfd );
                    throw std::string("Problem reading byte stream");
                    }
                cumsum += bytes;
            } while ( bytes > 0 );

            close( connectionfd );
            if ( cumsum == 0 )
                throw std::string("Empty message rec");
            buf[ cumsum ] = 0;
            
            std::string urlStr( buf, cumsum );
            ::vector< url_score > scores = deserializeScores( urlStr );
            std::cout << "Rank received: \n";
            printRanks( scores );
            mergedScores = merge( mergedScores, scores );
            }

        void DoLoop( size_t threadID )
            {
            while ( true )
                {
                try
                    {
                    int sockfd = startServer();
                    while ( true )
                        {
                        collectResult( sockfd );
                        }
                    }
                catch ( std::string e )
                    {
                    std::cerr << e << std::endl;
                    }
                }
            }

    public:
        QueryServer( size_t qSize = 1024, int myPort = 8888, size_t mSize = 10 ) 
            : ThreadPool( { "RankerManager", NUM_RANKERS, nullptr, 0, ThreadPool::LoopPool } ), 
                queueSize( qSize ), port( myPort ), mergeSize( mSize )
            { 
            std::cout << "input port = " << port << std::endl;
            MutexInit(&mutex, nullptr);
            }
        ~QueryServer( ) 
            { 
            }

        // decode the received message to obtain the query string;
        // called by the main server;
        std::string deserializeQueryMsg( std::string msg )
            {
            const char * search = "?query=";
            size_t start = msg.find( search );
            if ( start == std::string::npos )
                return "";
            start += strlen( search );
            return msg.substr(start);
            }

        // main server calls this function when it receives a query from the user
        // call deserializeQueryMsg before calling this function
        ::vector< url_score > CollectRanks( std::string& query )
            {
            CriticalSection s(&mutex);
            int sockfd = startServer();
            //++requestID;

            if ( mergedScores.size( ) > 0 )
                mergedScores = ::vector< url_score >();  // clear buffer

            size_t aliveRankers = 0;
            for ( size_t i = 0; i < NUM_RANKERS; ++i )
                {
                std::cout << "Sending to the " << i << "th machine\n";
                size_t attempts = 0;  // only allow 10 attemps to connect
                while ( sendQuery( i, query ) == -1 && attempts++ < 10 )
                    continue;
                if ( attempts < 10 )
                    ++aliveRankers;
                }
            
            for ( size_t i = 0; i < aliveRankers; ++i )
                {
                collectResult( sockfd );
                }

            close( sockfd );

            // Send the query to each ranker until we have received a response
            // from each of the rankers
            // bool allFinished = false;
            // while ( !allFinished )
            //     {
            //     allFinished = true;
            //     for ( size_t i = 0; i < NUM_RANKERS; ++i )
            //         {
            //         if ( !finished[ i ] )
            //             {
            //             //sendQuery( i , query );
            //             allFinished = false;
            //             }
            //         }
            //     if ( !allFinished )
            //         sleep( 10 );
            //     }

            return mergedScores;
            }

    };

