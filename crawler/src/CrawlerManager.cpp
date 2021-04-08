#include "CrawlerManager.h"

void ListenManager::DoTask( Task task, size_t threadID )
    {
    while ( alive )
        {
        // Listen on a socket for incoming links

        // Verify URL hash

        // Push the URL to the frontier
        }
    }

void SendManager::DoTask( Task task, size_t threadID )
    {
    Link *link = (Link *) task.args;
    // Send the link to the appropriate machine
    // machine ID = hash(URL) % N machines
    // If self, push to frontier
    if ( task.deleteArgs )
        delete link;
    }