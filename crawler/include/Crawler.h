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
#define NUM_OBJECTS 1000
#define FP_RATE 0.1
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

        void parseRobot( const String& robotUrl );

    private:
        Frontier *frontier;
        FileBloomfilter *visited;
        SendManager *manager;

        void DoTask( Task task, size_t threadID ) override;

        String retrieveWebpage( const ParsedUrl& url );

        // TODO: ( with the index team ) add words to index
        void addWordsToIndex( const HtmlParser& htmlparser );
    };
