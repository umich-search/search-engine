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
const int port = 8888;
const size_t queue_size = 1024;

static const char *Host[ numMachine ] = 
    {
    "172.21.159.128", 
    "192.168.2.11", 
    };

void makeSendAddr(struct sockaddr_in *addr, const char *hostname, int port);
void makeListenAddr(struct sockaddr_in *addr, int port);
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

class ListenManager : public CrawlerManager 
    {
    public:
        ListenManager( Init init, Frontier *frontier, FileBloomfilter *visited );
        ~ListenManager( );

    private:
        int socketFD;

        static void *submitThread( void *arg )
            {
            ( ( ListenManager * )arg )->submitConnection( );
            return nullptr;
            }
        void submitConnection( );
        void DoTask( Task task, size_t threadID ) override;
    };

class SendManager : public CrawlerManager
    {
    public:
        SendManager( Init init, Frontier *frontier, FileBloomfilter *visited )
            : CrawlerManager( init, frontier, visited ) { }
        ~SendManager( ) { }

    private:
        void DoTask( Task task, size_t threadID ) override;
    };