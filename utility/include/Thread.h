#pragma once
#include "mString.h"
#include "TaskQueue.h"
#include <pthread.h>
#include <atomic>
#include <iostream>

class Thread
    {
    public:
        Thread() { }

        ~Thread( ) { }

        // Returns true on successful thread creation
        bool Start( int threadID, TaskQueue *queue )
            {
            ID = threadID;
            taskQueue = queue;
            return pthread_create( &thread, NULL, ThreadStart, this ) == 0;
            }

        // Wait for this thread to exit
        int Join()
            {
            return pthread_join( thread, NULL );
            }

        // Get the ID of this thread
        int GetID()
            {
            return ID;
            }

    protected:
        // Override this function with the single task a thread should run
        virtual void DoTask( void *args ) = 0;

    private:

        int ID;
        pthread_t thread;
        TaskQueue *taskQueue;

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
            for( void *task = taskQueue->PopTask(); task != nullptr; task = taskQueue->PopTask() )
                {
                try 
                    {
                    DoTask( task );
                    }
                catch ( ... )
                    {
                    std::cerr << "Thread (" << ID << ") Exception" << std::endl;
                    }
                }
            }
    };