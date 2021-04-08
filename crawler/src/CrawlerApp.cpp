#include "CrawlerApp.h"

CrawlerApp::CrawlerApp( const Parameters &param )
    {
    MutexInit( &printMutex, nullptr );
    // Initialize the frontier
    frontier = Frontier( );
    visited = FileBloomfilter( );
    // Initialize the thread pools
    ThreadPool::Init init;
    init.printMutex = &printMutex;
    init.name = "ListenManager";
    init.numThreads = param.numListenThreads;
    listenManager = ListenManager( init, &frontier, &visited );
    init.name = "SendManager";
    init.numThreads = param.numSendThreads;
    sendManager = SendManager( init, &frontier, &visited );
    init.name = "Crawler";
    init.numThreads = param.numCrawlThreads;
    crawlers = Crawler( init, &frontier, &visited, &sendManager );
    }

void CrawlerApp::Start( )
    {
    // Start the manager thread pool
    listenManager.Start();
    sendManager.Start();
    // Start the crawler thread pool
    crawlers.Start();
    }

void CrawlerApp::Stop( )
    {
    // Stop crawling first
    crawlers.Stop();
    crawlers.Join();
    // Then stop URL management
    sendManager.Stop();
    listenManager.Stop();
    sendManager.Join();
    listenManager.Join();
    }

int main ( int argc, char **argv )
    {
    // TODO: take application parameters as a file? 
    if ( argc != 4 )
        {
        std::cerr << "Usage: " << argv[0] << "numCrawlThreads numListenThreads numSendThreads " 
            << "insert frontier params here" << std::endl;
        return 1;
        }
    CrawlerApp::Parameters params;
    params.numCrawlThreads = atoi( argv[1] );
    params.numListenThreads = atoi( argv[2] );
    params.numSendThreads = atoi( argv[3] );
    CrawlerApp app( params );

    // TODO: thread print window simultaneous to user input
    char input;
    std::cout << "Start crawling? (Y/N): ";
    std::cin >> input;
    if ( tolower( input ) == 'y' )
        app.Start();
    else
        return;
    input = 'n';
    while ( tolower( input ) != 'y' )
        {
        std::cout << "Stop crawling? (Y/N): ";
        std::cin >> input;
        }
    app.Stop();
    }