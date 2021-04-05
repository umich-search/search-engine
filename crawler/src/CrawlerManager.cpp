#include "CrawlerManager.h"

CrawlerManager::CrawlerManager( Frontier *front, size_t numCrawlers ) 
    : frontier( front ), crawlers( numCrawlers ) { }

void CrawlerManager::StartCrawl()
    {
    // Start the crawler threads async
    for ( size_t i = 0; i < crawlers.size(); ++i )
        {
        crawlers[i].Start( i, &crawlerTaskQueue );
        }
    // Start the manager thread async
    Start( crawlers.size(), &managerTaskQueue );
    }

void CrawlerManager::HaltCrawl()
    {
    managerTaskQueue.Halt(); // stop pulling from frontier
    crawlerTaskQueue.Halt();
    for ( size_t i = 0; i < crawlers.size(); ++i )
        {
        crawlers[i].Join();
        }
    }

void CrawlerManager::DoTask( Task *task )
    {
    while ( task->ExitStatus() == false || frontier->Empty() )
        {
        // Get a URL from the frontier
        String url = frontier->PopUrl();
        // Put the URL on the heap
        String *args = new String(url);
        // Send the task to the pool of crawler threads
        crawlerTaskQueue.PushTask((void *) args, true );
        // URL will be freed from heap in crawler thread
        }
    }

// int main( int argc, char **argv )
//     {
//     if ( argc != 4 )
//         {
//         std::cout << "Usage: " << argv[0] << " seedFile urlQueueFile numCrawlers \n";
//         return 1; 
//         }
//     CrawlerManager crawlerManager( argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]) );
//     char response;
//     std::cout << "Start crawling? (Y/N): ";
//     std::cin >> response;
//     if ( tolower( response ) == 'y' )
//         crawlerManager.start();
//     }
