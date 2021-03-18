#pragma once
#include "../utility/string.h"
#include "../utility/PriorityQueue.h"
#include "../utility/Concurrency.h"
#include "../utility/Vector.h"
#include "../utility/Thread.h"
#include "parser/HtmlParser.h"
// TODO: #include "get-url/LinuxGetSsl.h" 
// Create a get-url class like HtmlParser
#include <atomic>

// Breadth-first search: visiting URLs in the order they enter the frontier (queue)
// - if PQ empty refresh with 10 random from pool from oldest N/10
// - if N is large, it could contain a link we most recently pushed
// Depth-first search: visiting the most recently encountered URLs (stack)


class Frontier
    {
    // TODO: define a custom comparison symbol
    PriorityQueue< const Link > urlPq( );
    
    // TODO: int fd = url pool on disk
    mutex_t frontMutex;
    cv_t frontWait;
    
    public:
        // ctor from seeds file "seeds" and the file for url pool "urlPool" 
        Frontier( const char *seeds, const char *urlQueue, size_t pqSize = 10 );

        ~Frontier( );

        // if not already seen, push the url to the pool of urls
        void PushUrl( Link );
        
        // If pq size == 0, refresh with 10 random urls from pool
        // block until something shows up in the frontier
        const Link GetUrl( );

        // returns true if the pq size == 0
        bool empty();
    };

// class instance per crawler thread
class Crawler : public Thread
    {
    size_t id;
    Frontier *frontier;
    // TODO: HashTable to store robot.txt files
    
    public:
        Crawler( );

        ~Crawler( );

        // These had to be moved out of the constructor because the crawler
        // doesn't have an assignment operator that can be used when
        // pushBack() is called on the vector<Crawler>
        void setParameters( size_t cralwerId, Frontier *front );

    private:
        // pack all crawler functionalities into one function
        // an infinite loop consisting of steps of poping urls, retrievingWebpages,
        // waiting if needed, parsing the retrieved file, and adding to the index
        // If the popped url is the "halting" url, stop looping
        void Work( );

        // get a url from the frontier
        // check the url, call LinuxGetSsl or LinuxGetUrl based on the protocol
        String retrieveWebpage( const Link& url );

        // parse the robot.txt
        // mark urls as unreachable in the frontier
        String parseRobot( const String& robotFile );

        // add not-seen links the frontier
        void addLinksToFrontier( const HtmlParser& htmlparser );

        // TODO: ( with the index team ) add words to index
        void addWordsToIndex( const HtmlParser& htmlparser );
    };
