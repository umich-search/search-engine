#pragma once
#include "Crawler.h"
#include "TaskQueue.h"

// ----- CrawlerManager.h
// Task Input: None (run forever until interrupted)
// Task: Pull URLs from the frontier and distribute to crawlers
// Task Output: URLs to crawlers (via crawlerTaskQueue)

class CrawlerManager : public Thread
    {
    public:
        CrawlerManager( const Thread::Init &init, Frontier *frontier, TaskQueue *crawlerTaskQueue )
            : Thread( init ), frontier( frontier ), crawlerTaskQueue( crawlerTaskQueue ) { }

        ~CrawlerManager( );

    private:   
        void DoTask( TaskQueue::Task *task ) override;

        Frontier *frontier;
        TaskQueue *crawlerTaskQueue;
    };
