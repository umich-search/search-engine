#pragma once
#include <string.h>
#include <PriorityQueue.h>
#include <Concurrency.h>
#include <Vector.h>
#include <Thread.h>
#include "BloomFilter.h"
#include "HtmlParser.h"
#include "GetUrl.h"
#include "Frontier.h"
#define NUM_OBJECTS 1000
#define FP_RATE 0.1
static const char *ROBOT_FILE = "robots.txt";

// ----- Crawler.h
// Task Input: One URL from crawler manager
// Task: Download the HTML, parse the HTML, add the parsed words to the index
// Task Output: Parsed URLs to link manager (via linkTaskQueue)

class Crawler : public Thread
    {
    public:
        Crawler( const Thread::Init &init );

        ~Crawler( );

        void parseRobot( const String& robotUrl );

    private:
        TaskQueue *linkTaskQueue;

        void DoTask( TaskQueue::Task *task ) override;

        String retrieveWebpage( const ParsedUrl& url );

        // TODO: ( with the index team ) add words to index
        void addWordsToIndex( const HtmlParser& htmlparser );
    };
