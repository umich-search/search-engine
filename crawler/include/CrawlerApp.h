#pragma once
#include "Crawler.h"
#include "CrawlerManager.h"
#include "LinkManager.h"

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

        CrawlerApp( const Parameters &param )
            {
            MutexInit( &printMutex, nullptr );
            // Initialize the frontier
            frontier = Frontier( );
            // Initialize the threads
            Thread::Init init;
            init.ID = 0;
            init.printMutex = &printMutex;
            // Initialize the crawler manager thread
            init.taskQueue = &managerTaskQueue;
            crawlerManager = CrawlerManager( init, &frontier, &crawlerTaskQueue );
            // Initialize the crawler threads
            init.taskQueue = &crawlerTaskQueue;
            for ( size_t i = 0; i < param.numListenThreads; ++i )
                {
                crawlers.pushBack( Crawler( init ) );
                init.ID++;
                }
            // Initialize the link manager threads
            init.taskQueue = &linkTaskQueue;
            for ( size_t i = 0; i < param.numListenThreads; ++i )
                {
                linkManagers.pushBack( LinkManager( init, true ) );
                init.ID++;
                }
            for ( size_t i = 0; i < param.numSendThreads; ++i )
                {
                linkManagers.pushBack( LinkManager( init, false ) );
                init.ID++;
                }
            }

        ~CrawlerApp( ) { }

        void Start()
            {
            // Start the crawler manager thread
            crawlerManager.Start();

            // Start the crawler threads
            for ( size_t i = 0; i < crawlers.size(); ++i )
                crawlers[i].Start();

            // Start the link manager threads
            for ( size_t i = 0; i < linkManagers.size(); ++i )
                linkManagers[i].Start();
        
            // Start crawling by tasking crawler manager
            managerTaskQueue.PushTask( nullptr, false );
            }

        void Stop()
            {
            // Stop the crawler manager
            managerTaskQueue.Halt();
            crawlerManager.Join();

            // Stop the crawlers
            crawlerTaskQueue.Halt();
            for ( size_t i = 0; i < crawlers.size(); ++i )
                {
                crawlers[i].Join();
                }

            // Stop the link managers
            linkTaskQueue.Halt();
            for ( size_t i = 0; i < linkManagers.size(); ++i )
                {
                linkManagers[i].Join();
                }
            }

    private:
        mutex_t printMutex;
        // TODO: add the bloom filter to frontier class?
        // If frontier has seen url, don't add
        // That way only user needs to call frontier.PushUrl()
        Frontier frontier;
        FileBloomfilter visited;

        // Crawler manager thread
        CrawlerManager crawlerManager;
        TaskQueue managerTaskQueue;
        // Crawler thread pool
        vector< Crawler > crawlers;
        TaskQueue crawlerTaskQueue;
        // Link manager thread pool
        vector< LinkManager > linkManagers;
        TaskQueue linkTaskQueue;
    };