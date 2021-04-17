#include "CrawlerApp.h"

// -- Crawler App Parameters
const size_t NUM_CRAWL_THREADS = 200;
const size_t NUM_SEND_THREADS = 1;
const size_t NUM_LISTEN_THREADS = 1;
const size_t NUM_DISK_QUEUE = 1000;
const size_t PQ_SIZE = 1000;
const int NUM_OBJECTS = 100000;
const double FP_RATE = 0.0001;
const char * FRONTIER_DIR = "frontier";
const char * BLOOMFILTER_FILE = "bloomfilter";

CrawlerApp::CrawlerApp( size_t machineID, bool frontierInit )
    : frontier( 
        FRONTIER_DIR, 
        NUM_DISK_QUEUE, 
        PQ_SIZE ),
    visited( 
        BLOOMFILTER_FILE, 
        NUM_OBJECTS, 
        FP_RATE ),
    listenManager( 
        { "ListenManager", NUM_LISTEN_THREADS, &printMutex, machineID, ThreadPool::LoopPool },
        &frontier, &visited ),
    sendManager(
        { "SendManager", NUM_SEND_THREADS, &printMutex, machineID, ThreadPool::TaskPool },
        &frontier, &visited ),
    crawlers(
        { "Crawler", NUM_CRAWL_THREADS, &printMutex, machineID, ThreadPool::LoopPool },
        &frontier, &visited, &sendManager )
    {
    std::cout << "Constructing Crawler App (machineID:" << machineID << ")..." << std::endl;
    MutexInit( &printMutex, nullptr );
    if ( frontierInit ) 
        {
        //String seedFile = "seedlist/test.txt";
        String seedFile = String("seedlist/seedM") + ltos(machineID) + String(".txt");
        std::cout << "Constructing frontier using seed list..." << std::endl;
        frontier.FrontierInit( seedFile.cstr(), &visited );
        }
    }

CrawlerApp::~CrawlerApp( )
    {
    Stop( );
    MutexDestroy( &printMutex );
    }

void CrawlerApp::Start( )
    {
    std::cout << "Starting Crawler App..." << std::endl;
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