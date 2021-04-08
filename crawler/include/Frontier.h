// NOTE: need to compile with HashTable.cpp or provide another hash function in Frontier.cpp

#pragma once

#include <iostream>
#include <iostream>
#include <chrono>
#include <string.h>
#include "PriorityQueue.h"
#include "Concurrency.h"
#include "DiskQueue.h"
#include "Vector.h"
#include "GetUrl.h"
#include "HtmlParser.h"

// TODO: to use anchor text for priority, we can provide a priority
// calculator that returns the priority score within 0 to pool.size( ) - 1
class Frontier
    {
    private:
        struct url_t
            {
            String url;
            bool operator== ( const url_t& other ) const;
            bool operator< ( const url_t& other ) const;
            bool operator<= ( const url_t& other ) const;
            url_t( );
            url_t( const String& );
            };
        String rootDir;  // root dir of the frontier
        vector< DiskQueue * > urlPool;  // disk storage of urls
        PriorityQueue< url_t > urlPq;
        // I think its save to not use a mutex over the entire vector
        // as the vector is fixed in size during construction, so there
        // should not be reallocations ( namely threads only calls [] to 
        // the vector )
        vector< mutex_t * > poolMutexes;
        mutex_t pqMutex;
        int ( *priorityCalculator )( const Link& );
        size_t pqSize;
        // private methods:
        // select an index of the queues randomly 
        // TODO: may be biased
        int randomSelect( ) const;
        // link the queues to the root directory
        // urlPool should first be filled with NULL
        void linkToRoot( const char * );

    public:
        // ctor in the beginning, specify the number of disk queues
        // const char *: name of the directory containing diskqueues
        // size_t: number of disk queues
        // size_t: size of the priority queue
        // int ( * )( const Link& ): priority calculator
        Frontier( const char *, size_t, size_t, int ( * )( const Link& ) = nullptr );
        // dtor
        ~Frontier( );
        // initialize the frontier with the seed list file
        void FrontierInit( const char *seedFile );
        // push the url into the frontier; the url has to be unseen
        void PushUrl( Link& );
        // return the top url in the urlPq
        // if urlPq is empty, refills it with random sampling
        String PopUrl( );

        // TODO
        bool Empty( ) { return true; }
    };