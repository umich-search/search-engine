#pragma once
#include <Concurrency.h>
#include "Crawler.h"

class CrawlerManager
    {
    vector< Crawler > crawlers;
    Frontier frontier;
    FileBloomfilter visited;

    public:
        // Create all threads 
        CrawlerManager( 
            // frontier
            const char *dir, size_t numq, size_t pqsize, 
            // bloom filter
            const char *filename, int num_objects, double false_positive_rate,
            // crawlers
            size_t numCrawlers = 100 );

        // destroy the threads
        ~CrawlerManager( );

        // start the crawling process
        void start( );

        // block until the frontier is empty, namely joining all thread workers,
        // then broadcast all threads to stop working
        void halt( );
    };
