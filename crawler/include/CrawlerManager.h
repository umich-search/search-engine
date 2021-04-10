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

const size_t numMachine = 7;
const int port = 0;
const size_t queue_size = 1024;

const char *Host[ numMachine ] = 
    {
    "127.0.0.0",
    "127.0.0.1",
    "127.0.0.2",
    "127.0.0.3",
    "127.0.0.4",
    "127.0.0.5",
    "127.0.0.6",
    };

void makeSendAddr(struct sockaddr_in *addr, const char *hostname, int port);
void makeListenAddr(struct sockaddr_in *addr, int port);
int getPort( int sockFd );

class CrawlerManager : public ThreadPool
    {
    public:
        CrawlerManager( Init init, Frontier *frontier, FileBloomfilter *visited )
            : ThreadPool( init ), frontier( frontier ), visited( visited ) { }
        ~CrawlerManager( );

    protected:
        size_t myIndex;
        Frontier *frontier;
        FileBloomfilter *visited;
    };

class ListenManager : public CrawlerManager 
    {
    public:
        ListenManager( Init init, Frontier *frontier, FileBloomfilter *visited )
            : CrawlerManager( init, frontier, visited ) 
            {
            socketFD = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
            int yesval = 1;
            setsockopt( socketFD, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof( yesval ) );

            struct sockaddr_in addr;
            makeListenAddr( &addr, port );
            bind( socketFD, ( sockaddr * ) &addr, sizeof( addr ) );
            // int myPort = getPort( socketFD );
            listen( socketFD, queue_size );
            }

        ~ListenManager( )
            {
            close( socketFD );
            }

    private:
        int socketFD;
        void DoTask( Task task, size_t threadID ) override;
    };

class SendManager : public CrawlerManager
    {
    public:
        SendManager( Init init, Frontier *frontier, FileBloomfilter *visited )
            : CrawlerManager( init, frontier, visited ) { }
        ~SendManager( );

    private:
        void DoTask( Task task, size_t threadID ) override;
    };