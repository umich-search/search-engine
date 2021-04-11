#pragma once
#include <string.h>
#include <PriorityQueue.h>
#include <Concurrency.h>
#include <Vector.h>
#include "CrawlerManager.h"
#include "ThreadPool.h"
#include "BloomFilter.h"
#include "HtmlParser.h"
#include "GetUrl.h"
#include "Frontier.h"
#include "IndexConstructor.h"
static const char *ROBOT_FILE = "robots.txt";

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
        IndexConstructor indexConstructor;

        void DoTask( Task task, size_t threadID ) override;

        String retrieveWebpage( const ParsedUrl& url );
        void parseRobot( const String& robotUrl );

        // TODO: ( with the index team ) add words to index
        void addWordsToIndex( const HtmlParser& htmlparser, String url, size_t threadID );
    };
