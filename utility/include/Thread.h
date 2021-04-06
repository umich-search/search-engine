#pragma once
#include "mString.h"
#include "TaskQueue.h"
#include "Concurrency.h"
#include <pthread.h>
#include <atomic>
#include <iostream>

class Thread
    {
    public:
        struct Init
            {
            int ID;
            TaskQueue *taskQueue;
            mutex_t *printMutex;
            };

        Thread( const Init &init ) 
            : ID( init.ID ), taskQueue( init.taskQueue ), printMutex( init.printMutex ) { }

        ~Thread( ) { }

        // Returns true on successful thread creation
        bool Start()
            {
            return pthread_create( &thread, NULL, ThreadStart, this ) == 0;
            }

        // Wait for this thread to exit
        int Join()
            {
            return pthread_join( thread, NULL );
            }

    protected:
        TaskQueue *taskQueue;

        // Override this function with the single task a thread should run
        virtual void DoTask( TaskQueue::Task *task ) = 0;

        int GetID()
            {
            return ID;
            }

        void Print( std::string output )
            {
            Lock(printMutex);
            std::cout << "Thread (ID:#" << GetID() << "): " << output << std::endl;
            Unlock(printMutex);
            }

    private:
        int ID;
        pthread_t thread;
        mutex_t *printMutex;

        // Entry point for the pthread
        static void * ThreadStart( void *context ) 
            {
            ( (Thread *) context )->Work();
            return NULL;
            }

        // Infinite loop that waits for tasks from the task queue
        // TODO: Linux Signal Handler? 
        // A signal to terminate a thread will terminate the
        // entire process, losing the frontier/index in memory 
        void Work()
            {
            TaskQueue::Task *task;
            for( task = taskQueue->PopTask(); task != nullptr; task = taskQueue->PopTask() )
                {
                try 
                    {
                    DoTask( task );
                    }
                catch ( ... )
                    {
                    Print(std::string("Exception"));
                    }
                delete task;
                }
            }
    };