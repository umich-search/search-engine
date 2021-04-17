#pragma once
#include "Crawler.h"
#include "CrawlerManager.h"

class CrawlerApp
    {
    public:
        CrawlerApp( size_t machineID, bool frontierInit );
        ~CrawlerApp( );

        void Start();
        void Stop();
        void Join();

    private:
        mutex_t printMutex;
        Frontier frontier;
        FileBloomfilter visited;
        
        ListenManager listenManager; // single-threaded
        SendManager sendManager; // multi-threaded
        Crawler crawlers; // multi-threaded
    };