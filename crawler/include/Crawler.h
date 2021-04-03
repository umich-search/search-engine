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
static const char *ROBOT_FILE = "robot.txt";

class Crawler : public Thread
    {
    Frontier *frontier;
    // TODO: HashTable to store robot.txt files
    Bloomfilter *disallowedUrl;

    public:
        Crawler( );

        ~Crawler( );

        void parseRobot( const String& robotUrl );

    private:

        void DoTask( void *args ) override;

        String retrieveWebpage( const ParsedUrl& url );

        // parse the robot.txt
        // mark urls as unreachable in the frontier
        // void parseRobot( const String& robotUrl );

        // add not-seen links the frontier
        void addLinksToFrontier( HtmlParser& htmlparser );

        // TODO: ( with the index team ) add words to index
        void addWordsToIndex( const HtmlParser& htmlparser );
    };
