#include "CrawlerManager.h"

CrawlerManager::CrawlerManager( const char *seeds, const char *urlQueue, size_t numCrawlers ) 
    : frontier( seeds, urlQueue ), crawlers( numCrawlers ) { }

void CrawlerManager::start()
    {
    for ( size_t i = 0; i < crawlers.size(); ++i )
        {
        crawlers[i].setParameters( i, &frontier );
        crawlers[i].Start();
        }
    }

void CrawlerManager::halt()
    {
    for ( size_t i = 0; i < crawlers.size(); ++i )
        {
        crawlers[i].Kill();
        }
    }

int main( int argc, char **argv )
    {
    if ( argc != 4 )
        {
        std::cout << "Usage: " << argv[0] << " seedFile urlQueueFile numCrawlers \n";
        return 1; 
        }
    CrawlerManager crawlerManager( argv[1], argv[2], atoi(argv[3]) );
    char response;
    std::cout << "Start crawling? (Y/N): ";
    std::cin >> response;
    if ( tolower( response ) == 'y' )
        crawlerManager.start();
    }