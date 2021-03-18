// Design the frontier that satisfy both complexity and politeness to some appropriate degree

// there are whopping 1517 domain extensions, or TLDs;
// country-code TLDs ( ccTLD ) made up of half of currently registered domain names, over 161.8 millions in Q3 2019;
// Top 10 TLDs: .com, .tk, .cn, .de, .net, .uk, .org, .tw, .nl, .ru; .tk, .cn, .de, .uk, .tw, .nl, .ru are ccTLDs.
// THOUGHTS: to ensure the politeness by having only one thread crawling one host at the given time, maintaining 100 
// threads and a frontier of size 2048 might be a good choice. The probability of the same domain showing up in the 
// priority queue at the same time is very low, the probability of distributing them to two working threads at the same time 
// is even ten times lower. Question is if the url is 400 bytes, then it would take up 800KB in memory.
// reference: https://makeawebsitehub.com/how-many-domains-are-there/

// Google has indexed tens of billions of pages and trillions of urls;
// Enforcing politeness requires setting the possible access time for each domain
// reference: https://michaelnielsen.org/ddi/how-to-crawl-a-quarter-billion-webpages-in-40-hours/


#pragma once 

#include <iostream>
#include <chrono>
#include "../utility/Vector.h"
#include "../utility/string.h"
#include "../utility/PriorityQueue.h"
#include "../utility/Concurrency.h"
#include "../utility/Vector.h"
#include "parser/HtmlParser.h"

// Breadth-first search: visiting the most recently encountered URLs (stack)
// - if PQ empty refresh with 10 random from pool from oldest N/10
// - if N is large, it could contain a link we most recently pushed
// Depth-first search: visiting all URLs in order (queue)

const unsigned int URL_LENGTH_IN_BYTES = 400; 

class FrontEndQueue
    {

    };

class Frontier
    {
    // TODO: define a custom comparison symbol
    PriorityQueue< const Link > urlPq( );
    
    // TODO: int fd = url pool on disk
    mutex_t frontMutex;
    cv_t frontWait;
    
    public:
        // ctor from seeds file "seeds" and the file for url pool "urlPool" 
        Frontier( const char *seeds, const char *urlPool, size_t pqSize = 10 );

        ~Frontier( );

        // if not already seen, push the url to the pool of urls
        void PushUrl( Link );
        
        // If pq size == 0, refresh with 10 random urls from pool
        // block until something shows up in the frontier
        const Link GetUrl( );
    };
