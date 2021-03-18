#pragma once
#include "../utility/Concurrency.h"
#include "crawler.h"

class CrawlerManager
    {
    vector< Crawler > crawlers;
    vector< pthread_t * > workers;
    Frontier frontier;

    public:
        // Create all threads 
        CrawlerManager( const char *seeds, const char *urlQueue, size_t numWorker = 100 );

        // destroy the threads
        ~CrawlerManager( );

        // block until the frontier is empty, namely joining all thread workers,
        // then broadcast all threads to stop working
        void Halt( );
    };
