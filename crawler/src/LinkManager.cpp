#include "LinkManager.h"

LinkManager::LinkManager( const Thread::Init &init, Frontier *frontier, 
                        FileBloomfilter *visited, bool isListener )
    : Thread( init ), frontier( frontier ), visited ( visited ), isListener( isListener )
        { }

void LinkManager::DoTask( TaskQueue::Task *task )
    {
    if ( isListener )
        {
        while ( !taskQueue->IsHalted() )
            {
            // Listen on socket for incoming URLs from other machines
            
            // Verify URL hash

            // Push the URL to frontier

            }

        }
    else // else, sender
        {
        // Check the URL hash and send to appropriate machine
        Link *link = (Link *) task->args;


        }
    }