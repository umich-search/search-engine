#pragma once
#include <string.h>
#include <PriorityQueue.h>
#include <Concurrency.h>
#include <Vector.h>
#include <fstream>
#include <ctime>
#include "CrawlerManager.h"
#include "ThreadPool.h"
#include "BloomFilter.h"
#include "HtmlParser.h"
#include "GetUrl.h"
#include "Frontier.h"
#include "IndexConstructor.h"
#include "Timer.h"
static const char *ROBOT_FILE = "robots.txt";
static const char *DOC_COUNT_FILE = "documentCount";
static const int PRINT_INTERVAL = 100; // print every this number of documents

// ----- Crawler.h
// Task Input: NONE
// Task: Pull URL from frontier, Download the HTML, parse the HTML,
//       add the parsed words to the index
// Task Output: Parsed URLs to manager

class Crawler : public ThreadPool
    {
    public:
        Crawler( Init init, Frontier *froniter, FileBloomfilter *visited, SendManager *manager );
        ~Crawler( );

    private:
        Frontier *frontier;
        FileBloomfilter *visited;
        SendManager *manager;
        std::atomic< size_t > documentCount;
        ThreadSafeTimer timer;
        IndexConstructor ic;
        mutex_t indexMutex;

        void DoLoop( size_t threadID ) override;
        void Crawl( size_t threadID );

        String retrieveWebpage( const ParsedUrl& url );
        void parseRobot( const String& robotUrl );
        void addWordsToIndex( const HtmlParser& htmlparser, String url, size_t threadID );
    };
