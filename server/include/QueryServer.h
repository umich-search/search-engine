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

#define MAX_MESSAGE_SIZE 10240

const size_t NUM_RANKERS = 1;
const size_t MAX_ATTEMPTS = 10;

static const char *RANKER_HOST[ NUM_RANKERS ] = 
    {
<<<<<<< HEAD
=======
        // "35.202.123.51",
>>>>>>> 2286fb0 (cloud debugging in process)
        //"35.202.123.51",
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
const int RANKER_PORT = 8888;

// local use
// const int RANKER_PORT[ ] = 
//     {
//     8000, 8001, 8002
//     };

// query server, handles and manages input queries and ranking results
class QueryServer : public ThreadPool
    {
    private:
        mutex_t mutex;
        mutex_t printMutex;
        size_t queueSize;

        ::vector< url_score > mergedScores;
        const size_t mergeSize;  // define the maximum size of the mergedScores

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

        int sendQuery( size_t machineID, std::string query )
            {
            int sockFD = socket( AF_INET, SOCK_STREAM, 0 );
            if ( sockFD == -1 )
                throw std::string("Unable to open stream socket");

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            inet_pton( AF_INET, RANKER_HOST[ machineID ] , &addr.sin_addr );
            // TODO: change for local versus cloud testing
            addr.sin_port = htons( RANKER_PORT ); // cloud
            //addr.sin_port = htons( RANKER_PORT[ machineID ] ); // local

            std::string address( RANKER_HOST[ machineID ] );
            address += std::string(":") + to_string( ntohs( addr.sin_port ) );
            std::string machine = to_string( machineID ) + std::string(" (") + address + std::string(")");

            if ( connect( sockFD, ( sockaddr * ) &addr, sizeof( addr ) ) == -1)
                {
                close( sockFD );
                throw std::string("Connection failed to machine: ") + machine;
                }

            // query += '\0';
            int bytes = send( sockFD, query.c_str(), query.size() + 1, 0 );
            if ( bytes == -1 )
                {
                close( sockFD );
                throw std::string("Sending bytes failed to machine: ") + machine;
                }

            return sockFD;
            }

        std::string getQueryResponse( int sockFD )
            {
            char buf[ MAX_MESSAGE_SIZE + 1 ];
            memset( buf, 0, MAX_MESSAGE_SIZE + 1 );
            size_t cumsum = 0;
            ssize_t bytes = 0;
            do {
                bytes = recv( sockFD, buf + cumsum, MAX_MESSAGE_SIZE - cumsum, 0);
                if (bytes == -1)
                    {
                    close( sockFD );
                    throw std::string("Problem reading byte stream");
                    }
                cumsum += bytes;
            } while ( bytes > 0 );

            if ( cumsum == 0 )
                {
                close( sockFD );
                throw std::string("Empty message received");
                }

            buf[ cumsum ] = 0;
            return std::string( buf, cumsum );
            }

        void queryRankServer( size_t machineID, std::string query )
            {   
            std::cout << "Sending query to machine: " << machineID << std::endl;
            int sockFD = sendQuery( machineID, query );
            std::cout << "Waiting for response from machine: " << machineID << std::endl;
            std::string response = getQueryResponse( sockFD );
            close( sockFD );
            ::vector< url_score > scores = deserializeScores( response );
            std::cout << "Rank received: \n";
            printRanks( scores );
            mergedScores = merge( mergedScores, scores );
            }

        void DoTask( Task task, size_t threadID )
            {
            std::string *query = (std::string *) task.args;
            size_t machineID = threadID;

            size_t attempts = 0;
            while ( attempts < MAX_ATTEMPTS )
                {
                try 
                    {
                    queryRankServer( machineID, *query );
                    break;
                    }
                catch ( std::string e )
                    {
                    String exception = "Exception: ";
                    exception += String( e.c_str() );
                    Print( exception, threadID );
                    sleep(2);
                    ++attempts;
                    }
                }

            if ( attempts == MAX_ATTEMPTS )
                {
                String output = "Too many attempts; ignoring rank results from machine: ";
                output += ltos( machineID );
                Print( output, threadID );
                }
            else 
                {
                String output = "Received rank results from machine: ";
                output += ltos( machineID );
                Print( output, threadID );
                }

            if ( task.deleteArgs )
                delete query;
            }

    public:
        QueryServer( size_t qSize = 1024, size_t mSize = 10 ) 
            : ThreadPool( { "QueryServer", NUM_RANKERS, &printMutex, 0, ThreadPool::TaskPool } ), 
                queueSize( qSize ), mergeSize( mSize )
            {
            MutexInit(&mutex, nullptr);
            MutexInit(&printMutex, nullptr);
            }
        ~QueryServer( ) 
            { 
            MutexDestroy(&mutex);
            MutexDestroy(&printMutex);
            }

        // main server calls this function when it receives a query from the user
        // call deserializeQueryMsg before calling this function
        ::vector< url_score > GetScores( std::string& query )
            {
            CriticalSection s(&mutex); // handle one query at a time
            std::cout << "--- BLOCKING: Serving query: " << query << std::endl;

            if ( mergedScores.size( ) > 0 )
                mergedScores = ::vector< url_score >();  // clear buffer

            Start(); // start the thread pool
            for ( size_t i = 0; i < NUM_RANKERS; ++i )
                {
                PushTask( (void *) new std::string(query), true );
                }
            Stop(); // stop the thread pool, waiting until finished
            
            return mergedScores;
            }
    };

