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

const size_t NUM_MACHINES = 2;
const int PORT = 8888;
const size_t QUEUE_SIZE = 1024;

static const char *HOST[ NUM_MACHINES ] = 
    {
        "localhost",
        "localhost",
    };

class CrawlerManager : public ThreadPool
    {
    public:
        CrawlerManager( Init init, Frontier *frontier, FileBloomfilter *visited )
            : ThreadPool( init ), frontier( frontier ), visited( visited ),
            myID( init.machineID ) { }
        ~CrawlerManager( ) { }

    protected:
        size_t myID;
        Frontier *frontier;
        FileBloomfilter *visited;
    };

class ListenManager : public CrawlerManager
    {
    public:
        ListenManager( Init init, Frontier *frontier, FileBloomfilter *visited, int port );
        ~ListenManager( );

    private:
        int startServer( size_t threadID );
        void runServer( int sockfd, size_t threadID );
        String handleConnect( int fd, size_t threadID );

        void DoLoop( size_t threadID ) override;
    };

class SendManager : public CrawlerManager
    {
    public:
        SendManager( Init init, Frontier *frontier, FileBloomfilter *visited, int port );
        ~SendManager( ) { }

    private:
        void sendURL( String url, size_t machineID, size_t threadID );

        void DoTask( Task task, size_t threadID ) override;
    };