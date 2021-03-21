#pragma once
#include "../utility/Concurrency.h"
#include "Crawler.h"

class CrawlerManager
    {
    vector< Crawler > crawlers;
    Frontier frontier;

    public:
        // Create all threads 
        CrawlerManager( const char *seeds, const char *urlQueue, size_t numCrawlers = 100 );

        // destroy the threads
        ~CrawlerManager( );

        // start the crawling process
        void start( );

        // block until the frontier is empty, namely joining all thread workers,
        // then broadcast all threads to stop working
        void halt( );
    };
