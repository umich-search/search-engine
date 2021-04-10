#pragma once
#include "ThreadPool.h"
#include "Frontier.h"
#include "BloomFilter.h"

// ----- CrawlerManager.h
// Task Input: URLs from crawler / URLs from other machines
// Task: Distribute URLs according to hash and obey bloom filter
// Task Output: URLs to frontier / URLs to other machines
const size_t numMachine = 7;
int port = 0;
size_t queue_size = 1024;

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
            : CrawlerManager( init, frontier, visited ) { }
        ~ListenManager( );

    private:
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