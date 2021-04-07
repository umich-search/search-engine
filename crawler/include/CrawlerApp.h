#pragma once
#include "Crawler.h"
#include "CrawlerManager.h"

class CrawlerApp
    {
    public:
        struct Parameters
        {
        size_t numCrawlThreads; // number of crawler threads
        size_t numListenThreads; // number of threads listening on sockets
        size_t numSendThreads; // number of threads sending on sockets
        // TODO: add frontier parameters
        };

        CrawlerApp( const Parameters &param );
        ~CrawlerApp( );

        void Start();
        void Stop();

    private:
        mutex_t printMutex;
        Frontier frontier;
        FileBloomfilter visited;

        ListenManager listenManager;
        SendManager sendManager;
        Crawler crawlers;
    };