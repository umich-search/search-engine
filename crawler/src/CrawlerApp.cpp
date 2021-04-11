#include "CrawlerApp.h"

// -- Crawler App Parameters
const size_t NUM_CRAWL_THREADS = 10;
const size_t NUM_SEND_THREADS = 10;
const size_t NUM_LISTEN_THREADS = 10;
const size_t NUM_DISK_QUEUE = 10;
const size_t PQ_SIZE = 8;
const int NUM_OBJECTS = 100000;
const double FP_RATE = 0.0001;
const char * FRONTIER_DIR = "frontier";
const char * BLOOMFILTER_FILE = "bloomfilter";
const char * SEEDLIST_FILE = "seedlist.txt";

CrawlerApp::CrawlerApp( )
    : frontier( 
        FRONTIER_DIR, 
        NUM_DISK_QUEUE, 
        PQ_SIZE ),
    visited( 
        BLOOMFILTER_FILE, 
        NUM_OBJECTS, 
        FP_RATE ),
    listenManager( 
        { "ListenManager", NUM_LISTEN_THREADS, &printMutex },
        &frontier, &visited ),
    sendManager(
        { "SendManager", NUM_SEND_THREADS, &printMutex },
        &frontier, &visited ),
    crawlers(
        { "Crawler", NUM_CRAWL_THREADS, &printMutex },
        &frontier, &visited, &sendManager )
    {
    std:cout << "Constructing Crawler App..." << std::endl;
    MutexInit( &printMutex, nullptr );
    frontier.FrontierInit( SEEDLIST_FILE );
    }

CrawlerApp::~CrawlerApp( )
    {
    Stop( );
    MutexDestroy( &printMutex );
    }

void CrawlerApp::Start( )
    {
    // Start the manager thread pool
    listenManager.Start();
    sendManager.Start();
    // Start the crawler thread pool
    crawlers.Start();
    for ( size_t i = 0; i < NUM_CRAWL_THREADS; ++i )
        crawlers.PushTask( nullptr, false );
    }

void CrawlerApp::Stop( )
    {
    // Stop crawling first
    crawlers.Stop();
    // Then stop URL management
    sendManager.Stop();
    listenManager.Stop();
    }

void CrawlerApp::Join()
    {
    sendManager.Join();
    listenManager.Join();
    crawlers.Join();
    }