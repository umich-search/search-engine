#pragma once
#include "Crawler.h"
#include "TaskQueue.h"

class CrawlerManager : public Thread
    {
    public:
        // Create all threads 
        CrawlerManager( Frontier *frontier, size_t numCrawlers = 100 );

        // destroy the threads
        ~CrawlerManager( );

        // start the crawling process
        void StartCrawl( );

        // block until the frontier is empty, namely joining all thread workers,
        // then broadcast all threads to stop working
        void HaltCrawl( );

    private:   
        void DoTask( void *args ) override;

        vector< Crawler > crawlers;
        Frontier *frontier;
        TaskQueue crawlerTaskQueue;
        TaskQueue managerTaskQueue;
    };
