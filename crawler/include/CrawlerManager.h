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

const size_t NUM_MACHINES = 14;
const int PORT = 8888;
const size_t QUEUE_SIZE = 1024;

static const char *HOST[ NUM_MACHINES ] = 
    {
        "35.202.123.51",
        "104.197.37.30",
        "35.221.26.91",
        "34.69.231.181",
        "34.66.107.136",
        "34.68.201.74",
        "35.188.164.185",
        "35.221.27.146",
        "35.199.41.233",
        "35.245.62.74",
        "34.86.92.34",
        "35.236.254.45",
        "35.230.186.30",
        "35.221.49.174",
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

class ConnectHandler : public CrawlerManager
    {
    public:
        ConnectHandler( Init init, Frontier *frontier, FileBloomfilter *visited );
        ~ConnectHandler( ) { }

    private:
        String handleConnect( int fd, size_t threadID );

        void incrementCountURL( size_t threadID );
        size_t countURL;
        mutex_t countURLmutex;

        void DoTask( Task task, size_t threadID ) override;
    };

class ListenManager : public CrawlerManager
    {
    public:
        ListenManager( Init init, Frontier *frontier, FileBloomfilter *visited,
                        size_t numListenThreads );
        ~ListenManager( );

    private:
        ConnectHandler connectHandler;

        int startServer( size_t threadID );
        void runServer( int sockfd, size_t threadID );

        void DoLoop( size_t threadID ) override;
    };

class SendManager : public CrawlerManager
    {
    public:
        SendManager( Init init, Frontier *frontier, FileBloomfilter *visited );
        ~SendManager( ) { }

    private:
        void incrementCountFailMachine( size_t machineID, size_t threadID );
        std::atomic<size_t> failedMachine[ NUM_MACHINES ];
        mutex_t failedMachineMutex;

        void sendURL( String url, size_t machineID );

        void DoTask( Task task, size_t threadID ) override;
    };