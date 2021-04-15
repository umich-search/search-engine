#pragma once
#include "Vector.h"
#include "Queue.h"
#include "Concurrency.h"
#include "mString.h"
#include <pthread.h>
#include <atomic>
#include <iostream>

class ThreadPool
    {
public:
    enum PoolType { TaskPool, LoopPool };
    struct Init
        {
        String name;
        size_t numThreads;
        size_t machineID;
        mutex_t *printMutex;
        PoolType type;
        Init( ) { };
        Init( String s, size_t numT, mutex_t *pm, size_t mID, PoolType type )
            : name( s ), numThreads( numT ), printMutex( pm ), machineID( mID ),
              type( type ) { }
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
    void Print( String output, size_t threadID );

    // --- Define the behavior of the thread pool here
    // TaskPool: Override this function with the single task a thread should run
    virtual void DoTask( Task task, size_t threadID ) { };
    // LoopPool: Override this function with the loop the thread should run
    virtual void DoLoop( size_t threadID ) { };

private:
    PoolType type;
    String name;
    size_t machineID;
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