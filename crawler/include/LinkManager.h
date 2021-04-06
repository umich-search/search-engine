#pragma once
#include "Thread.h"
#include "TaskQueue.h"
#include "BloomFilter.h"
#include "Frontier.h"

// ----- LinkManager.h
// Task Input: URLs from crawlers (via crawlerTaskQueue), 
//             URLs from other machines (via sockets)
// Task: Send/Receive URLs and distribute according to hash(URL)
// Task Output: URLs to local frontier, URLs to other machines

class LinkManager : public Thread
    {
    public:
        LinkManager( const Thread::Init &init, Frontier *frontier, 
                    FileBloomfilter *visited, bool isListener );

    private:
        Frontier *frontier;
        FileBloomfilter *visited;
        bool isListener; // Listen on socket, or send on socket

        void DoTask( TaskQueue::Task *task ) override;
    };