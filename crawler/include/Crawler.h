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

class Crawler : public Thread
    {
    size_t id;
    Frontier *frontier;
    // TODO: HashTable to store robot.txt files
    FileBloomfilter *visited;

    public:
        Crawler( );

        ~Crawler( );

        void parseRobot( const String& robotUrl );

        // These had to be moved out of the constructor because the crawler
        // doesn't have an assignment operator that can be used when
        // pushBack() is called on the vector<Crawler>
        void setParameters( size_t cralwerId, Frontier *front, FileBloomfilter *bf );

    private:
        // pack all crawler functionalities into one function
        // an infinite loop consisting of steps of poping urls, retrievingWebpages,
        // waiting if needed, parsing the retrieved file, and adding to the index
        // If the popped url is the "halting" url, stop looping
        void Work( );

        // check the url, call LinuxGetSsl or LinuxGetUrl based on the protocol
        String retrieveWebpage( const ParsedUrl& url );

        // parse the robot.txt
        // mark urls as unreachable in the frontier
        // void parseRobot( const String& robotUrl );

        // add not-seen links the frontier
        void addLinksToFrontier( HtmlParser& htmlparser );

        // TODO: ( with the index team ) add words to index
        void addWordsToIndex( const HtmlParser& htmlparser );
    };
