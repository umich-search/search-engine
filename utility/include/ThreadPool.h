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
    struct Task
        {
        Task( void *args, bool deleteArgs )
            : args( args ), deleteArgs( deleteArgs ) { }

        Task( )
            : args( nullptr ), deleteArgs( false ) { }

        void *args;
        bool deleteArgs; // Should the tasked thread free the args from memory?
        };

    struct Init
        {
        String name;
        size_t numThreads;
        mutex_t *printMutex;
        Init( ) { };
        Init( String s, size_t numT, mutex_t *pm )
            : name( s ), numThreads( numT ), printMutex( pm )
            {
            }
        };

    // init.printMutex won't be initialized here
    ThreadPool( Init init )
        : name( init.name ), threads( init.numThreads ), printMutex( init.printMutex ), 
        alive( false ), threadID( 0 )
        {
        MutexInit( &mutex, nullptr );
        }

    // printMutex won't be destroyed here
    ~ThreadPool( )
        {
        MutexDestroy(&mutex);
        }

    ThreadPool& operator= ( const ThreadPool& other )
        {
        name = other.name;
        printMutex = other.printMutex;
        threads = vector< pthread_t >( other.threads.size() );
        }

    void Start()
        {
        CriticalSection s( &mutex );
        if ( alive )
            return;
        alive = true;
        for ( size_t i = 0; i < threads.size(); ++i )
            alive = alive && !pthread_create( &threads[i], NULL, ThreadStart, this );
        if ( !alive )
            {
            Join();
            threadID = 0;
            }
        }

    void Stop()
        {
        CriticalSection s( &mutex );
        taskQueue.WaitUntilEmpty();
        taskQueue.Block();
        alive = false;
        }

    void Join()
        {
        CriticalSection s( &mutex );
        for ( size_t i = 0; i < threads.size(); ++i )
            pthread_join( threads[i], NULL );
        }

    bool PushTask( void *args, bool deleteArgs )
        {
        CriticalSection s( &mutex );
        if ( !alive )
            return false;
        return taskQueue.Push( Task( args, deleteArgs ) );
        }
    
    bool IsAlive()
        {
        return alive;
        }

protected:
    std::atomic< bool > alive;
    // Override this function with the single task a thread should run
    virtual void DoTask( Task task, size_t threadID ) = 0;

    void Print( std::string output, size_t threadID )
        {
        Lock(printMutex);
        std::cout << "Thread (" << name << ":" << threadID << "): " 
                  << output << std::endl;
        Unlock(printMutex);
        }

private:
    // Entry point for the pthread
    static void * ThreadStart( void *context ) 
        {
        ( (ThreadPool *) context )->Work();
        return NULL;
        }
    // Infinite loop that waits for tasks from the task queue
    // TODO: Linux Signal Handler? 
    // A signal to terminate a thread will terminate the
    // entire process, losing the frontier/index in memory 
    void Work()
        {
        size_t ID = threadID++;
        while ( alive ) 
            {
            Task task;
            if ( taskQueue.Pop( task ) )
                {
                try 
                    {
                    DoTask( task, ID );
                    }
                catch ( ... )
                    {
                    Print(std::string("Exception"), ID );
                    }
                }
            }
        }
    
    String name;
    mutex_t *printMutex;
    mutex_t mutex;

    vector< pthread_t > threads;
    std::atomic< size_t > threadID;
    Queue< Task > taskQueue;

    };