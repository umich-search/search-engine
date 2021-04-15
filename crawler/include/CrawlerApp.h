#pragma once
#include "Crawler.h"
#include "CrawlerManager.h"

class CrawlerApp
    {
    public:
        CrawlerApp( size_t machineID, bool frontierInit, int listenPort, int sendPort );
        ~CrawlerApp( );

        void Start();
        void Stop();
        void Join();

    private:
        mutex_t printMutex;
        Frontier frontier;
        FileBloomfilter visited;

        ListenManager listenManager;
        SendManager sendManager;
        Crawler crawlers;
    };