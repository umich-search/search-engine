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
#include <Vector.h>
#include <string.h>
#include <PriorityQueue.h>
#include <Concurrency.h>
#include <Vector.h>
#include <DiskQueue.h>
#include "GetUrl.h"
#include "HtmlParser.h"

// Breadth-first search: visiting the most recently encountered URLs (stack)
// - if PQ empty refresh with 10 random from pool from oldest N/10
// - if N is large, it could contain a link we most recently pushed
// Depth-first search: visiting all URLs in order (queue)

const size_t NUM_PSCHEDULER = 20;
const size_t NUM_DSCHEDULER = 2048;

class Frontier
   {
   private:
      struct pair
         {
         size_t dsIdx;  // the index in the domain scheduler
         std::chrono::high_resolution_clock::time_point t;  // the time when that domain is accessible

         pair( size_t dsi ) : dsIdx( dsi ), t( std::chrono::high_resolution_clock::now( ) )
            {
            t += std::chrono::milliseconds( 70 );  // visit the same domain every 70ms
            }

         bool operator< ( const pair& other ) const
            {
            return t > other.t;  // early pair will be the top one
            }

         bool operator> ( const pair& other ) const
            {
            return t < other.t;
            }

         bool operator<= ( const pair& other ) const
            {
            return !( *this > other );
            }

         bool operator>= ( const pair& other ) const
            {
            return !( *this < other );
            }
         };

      // priority scheduler has 20 disk queues labeled with priority number
      // domain scheduler categorizes urls into different domains ( disk queues ). As long as the number of disk queues in the domain 
      // scheduler is far more than the number of threads, the efficiency is not harmed too much.
      vector< DiskQueue * > priorityScheduler, domainScheduler;
      // Sort the pair/domain according to the time of last access, the older the topper
      // O(logn) faster than round robin O(n), and can block if no item satisfies the time requirement, while round robin will loop until
      // a pair's time from last visit goes beyond 70ms.
      PriorityQueue< pair > timeScheduler;  // faster than round robin
      // necessary synchronization
      // priority scheduler -> ps; domain sheduler -> ds; time scheduler -> ts
      mutex_t psLock, dsLock, tsLock;  
      cv_t psWait, dsWait, tsWait;
      // attributes to control the random selector ( a separate thread )
      bool halt;
      pthread_t *rs;  // random selector
      mutex_t haltLock;
      cv_t haltWait;
      // Score the priority of the input url to the range 1 - 20 and insert the url to the corresponding priority scheduler queue
      // TODO: need more research on how to determine the priority score.
      int ( *priorityCalculator )( const Link& );

      // the function select the urls from ps to ds
      static void *randomSelector( void * );
      // link scheduler to the directory, the directory must exist
      void linkScheduler( vector< DiskQueue * >&, const char * );

   public:
      // seeds should be in the file ( common seeds are in several MB )
      // Link the ps and ds to their disk location
      // the structure of psDir and dsDir should be the same:
      // psDir( or dsDir )/diskqueue1Dir/fileblock1, fileblock2, ...
      //                  |- diskqueue2Dir/fileblock1, fileblock2, ...
      //                  |- diskqueue3Dir/fileblock1
      //                  |               |- fileblock2
      //                  |               |- fileblock3
      //                  |               |- ...
      //                  |- diskqueue4Dir/...
      //                  |- ...
      Frontier( const char *psDir, const char *dsDir, int (*)( const Link& ) );

      ~Frontier( );

      // if not already seen, push the url to the pool of urls
      void PushUrl( Link& );
      
      // If pq size == 0, refresh with 10 random urls from pool
      // block until something shows up in the frontier
      const String GetUrl( );      
   };
