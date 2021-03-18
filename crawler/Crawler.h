#pragma once

#include "../utility/string.h"
#include "../utility/PriorityQueue.h"
#include "../utility/Concurrency.h"
#include "../utility/Vector.h"
#include "parser/HtmlParser.h"
#include "Frontier.h"
// TODO: #include "get-url/LinuxGetSsl.h" 
// Create a get-url class like HtmlParser

// class instance per crawler thread
class Crawler
    {
    Frontier *frontier;
    HtmlParser parser;
    // TODO: HashTable to store robot.txt files
    
    public:
        Crawler( Frontier *frontier );

        ~Crawler( );

        // pack all crawler functionalities into one function
        // an infinite loop consisting of steps of poping urls, retrievingWebpages,
        // waiting if needed, parsing the retrieved file, and adding to the index
        // If the popped url is the "halting" url, stop looping
        void Work( );

    private:
        // get a url from the frontier
        // check the url, call LinuxGetSsl or LinuxGetUrl based on the protocol
        String retrieveWebpage( const String& url );

        // parse the robot.txt
        // mark urls as unreachable in the frontier
        String parseRobot( const String& robotFile );

        // parse the html returned by retrieveWebpage
        // add any links to the frontier
        void parseHtml( const String& HtmlPage );

        // TODO: ( with the index team ) add words to index
        void addWordsToIndex( );
    };

// responsible for creating crawler threads
class CrawlerManager
    {
    vector< Crawler > crawlers;
    vector< pthread_t * > workers;
    Frontier frontier;

    public:
        // Create all threads 
        CrawlerManager( const char *seeds, const char *urlQueue, size_t numWorker = 100 );

        // destroy the threads
        ~CrawlerManager( );

        // block until the frontier is empty, namely joining all thread workers,
        // then broadcast all threads to stop working
        void Halt( );
    };
