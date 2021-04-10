#pragma once
#include "Crawler.h"
#include "CrawlerManager.h"

class CrawlerApp
    {
    public:
        CrawlerApp( );
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