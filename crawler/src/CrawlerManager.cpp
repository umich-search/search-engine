#include "CrawlerManager.h"

void CrawlerManager::DoTask( TaskQueue::Task *task )
    {
    while ( !taskQueue->IsHalted() || !frontier->Empty() )
        {
        // Get a URL from the frontier
        String url = frontier->PopUrl();
        // Put the URL on the heap
        String *args = new String(url);
        // Send the task to the pool of crawler threads
        crawlerTaskQueue->PushTask((void *) args, true );
        // URL will be freed from heap in crawler thread
        }
    }