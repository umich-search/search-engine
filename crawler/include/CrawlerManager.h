#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include "ThreadPool.h"
#include "Frontier.h"
#include "BloomFilter.h"

// ----- CrawlerManager.h
// Task Input: URLs from crawler / URLs from other machines
// Task: Distribute URLs according to hash and obey bloom filter
// Task Output: URLs to frontier / URLs to other machines

const size_t numMachine = 2;
// const int port = 8888;
const size_t queue_size = 1024;

static const char *Host[ numMachine ] = 
    {
        "127.0.0.1",
        "127.0.0.1",
    };

void makeSendAddr( struct sockaddr_in *addr, const char *hostname, int port );
int makeListenAddr( struct sockaddr_in *addr, int port );
int getPort( int sockFd );

class CrawlerManager : public ThreadPool
    {
    public:
        CrawlerManager( Init init, Frontier *frontier, FileBloomfilter *visited )
            : ThreadPool( init ), frontier( frontier ), visited( visited ),
            myIndex( init.machineID ) { }
        ~CrawlerManager( ) { }

    protected:
        size_t myIndex;
        Frontier *frontier;
        FileBloomfilter *visited;
    };

// class ListenManager : public CrawlerManager 
//     {
//     public:
//         ListenManager( Init init, Frontier *frontier, FileBloomfilter *visited, int port );
//         ~ListenManager( );

        

//     private:
//         int socketFD;
//         int listenPort;
//         pthread_t *submitPtr;

//         static void *submitThread( void *arg )
//             {
//             std::cout << "strat the thread\n";
//             ( ( ListenManager * )arg )->submitConnection( );
//             return nullptr;
//             }
//         void submitConnection( );
//         void DoTask( Task task, size_t threadID ) override;
//     };

class ListenManager : public CrawlerManager
    {
    public:
        ListenManager( Init init, Frontier *frontier, FileBloomfilter *visited, int port );
        ~ListenManager( );

    private:
        int listenPort;
        pthread_t *threadPtr;

        int startServer( );
        int handleConnect( int fd );
        int parseHeader( String header );
        void parseReceived( String msg );
        static void *listenThread( void *arg )
            {
            if ( ( ( ListenManager * )arg )->startServer( ) == -1 )
                std::cout << "Server crashed!" << std::endl; 
            return nullptr;
            }
    };

class SendManager : public CrawlerManager
    {
    public:
        SendManager( Init init, Frontier *frontier, FileBloomfilter *visited, int port );
        ~SendManager( ) { }

    private:
        int sendPort;

        void DoTask( Task task, size_t threadID ) override;
    };