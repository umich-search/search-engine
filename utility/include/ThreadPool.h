#pragma once
#include "Vector.h"
#include "Queue.h"
#include "Concurrency.h"
#include <pthread.h>
#include <atomic>
#include <iostream>

class ThreadPool
    {
public:
    struct Init
        {
        std::string name;
        size_t numThreads;
        mutex_t *printMutex;
        };

    ThreadPool( Init init );
    ~ThreadPool( );
    ThreadPool& operator= ( const ThreadPool& other );

    void Start();
    void Stop();
    void Join();
    bool PushTask( void *args, bool deleteArgs );
    bool IsAlive();

protected:
    std::atomic< bool > alive;
    struct Task
        {
        void *args;
        bool deleteArgs; // Should the tasked thread free the args from memory?
        };

    void Print( std::string output, size_t threadID );

    // Override this function with the single task a thread should run
    virtual void DoTask( Task task, size_t threadID ) = 0;

private:
    std::string name;
    mutex_t *printMutex;
    mutex_t mutex;

    vector< pthread_t > threads;
    std::atomic< size_t > threadID;
    ThreadSafeQueue< Task > taskQueue;

    // Entry point for the pthread
    static void * ThreadStart( void *context ) 
        { ((ThreadPool *) context)->Work();  return NULL; }
    
    // Infinite loop that waits for tasks from the task queue
    // TODO: Linux Signal Handler? 
    // A signal to terminate a thread will terminate the
    // entire process, losing the frontier/index in memory 
    void Work();
    };