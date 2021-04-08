#pragma once
#include "ThreadPool.h"
#include "Frontier.h"
#include "BloomFilter.h"

// ----- CrawlerManager.h
// Task Input: URLs from crawler / URLs from other machines
// Task: Distribute URLs according to hash and obey bloom filter
// Task Output: URLs to frontier / URLs to other machines
size_t numMachine = 7;
int port = 0;
size_t queue_size = 1024;
int make_server_sockaddr(struct sockaddr_in *addr, int port) {
	// Step (1): specify socket family.
	// This is an internet socket.
	addr->sin_family = AF_INET;

	// Step (2): specify socket address (hostname).
	// The socket will be a server, so it will only be listening.
	// Let the OS map it to the correct address.
	addr->sin_addr.s_addr = INADDR_ANY;

	// Step (3): Set the port value.
	// If port is 0, the OS will choose the port for us.
	// Use htons to convert from local byte order to network byte order.
	addr->sin_port = htons(port);

	return 0;
}
class CrawlerManager : public ThreadPool
    {
    public:
        CrawlerManager( Init init, Frontier *frontier, FileBloomfilter *visited )
            : ThreadPool( init ), frontier( frontier ), visited( visited ) { }
        ~CrawlerManager( );

    private:   
        Frontier *frontier;
        FileBloomfilter *visited;
        size_t myIndex;
    };

class ListenManager : public CrawlerManager 
    {
    public:
        ListenManager( Init init, Frontier *frontier, FileBloomfilter *visited )
            : CrawlerManager( init, frontier, visited ) { }
        ~ListenManager( );

    private:
        void DoTask( Task task, size_t threadID ) override;
    };

class SendManager : public CrawlerManager
    {
    public:
        SendManager( Init init, Frontier *frontier, FileBloomfilter *visited )
            : CrawlerManager( init, frontier, visited ) { }
        ~SendManager( );

    private:
        void DoTask( Task task, size_t threadID ) override;
    };