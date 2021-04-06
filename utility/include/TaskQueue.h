#pragma once
#include "Concurrency.h"
#include <atomic>
#include <pthread.h>

class TaskQueue
    {
    public:
        struct Task
            {
            Task( void *args, bool deleteArgs )
                : args( args ), deleteArgs( deleteArgs ) { }

            void *args;
            bool deleteArgs; // Should the tasked thread free the args from memory?
            };
        
        struct QueueItem
            {
            QueueItem( Task *task, QueueItem *next )
                : task( task ), next( next ) { }

            Task *task;
            QueueItem *next;
            };

        TaskQueue() 
            {
            halted.store( false );
            front = nullptr;
            back = nullptr;
            MutexInit(&mutex, nullptr);
            CvInit(&cv, nullptr);
            }

        ~TaskQueue() 
            {
            while ( front != nullptr )
                {
                QueueItem *temp = front;
                front = front->next;
                delete temp;
                }
            MutexDestroy(&mutex);
            CvDestroy(&cv);
            }

        void PushTask( void *args, bool deleteArgs ) 
            { 
            // Create a task
            Task *task = new Task( args, deleteArgs );
            // Create a place in task queue
            QueueItem *item = new QueueItem( task, nullptr );
            // Add the task to the back of the queue
            Lock(&mutex);
            if ( front == nullptr )
                front = item;
            if ( back != nullptr )
                back->next = item;
            back = item;
            Signal(&cv);
            Unlock(&mutex);
            }

        // Wait for task if necessary. May return nullptr if queue halted.
        Task *PopTask( ) 
            {
            Lock(&mutex);
            while ( !IsHalted() && ( front == nullptr ) )
                {
                Wait(&cv, &mutex);
                }
            if ( front == nullptr )
                {
                Unlock(&mutex);
                return nullptr;
                }
            Task *task = front->task;
            QueueItem* temp = front;
            front = temp->next;
            delete temp;
            Broadcast(&cv);   
            Unlock(&mutex);
            return task;      
            }

        // Wait until the task queue is empty. 
        void WaitForEmptyQueue() 
            {
            Lock(&mutex);
            while ( !IsHalted() && ( front != nullptr ) )
                {
                Wait(&cv, &mutex);
                }
            Unlock(&mutex);
            }

        // Wait until all tasks are complete before halting.
        // Race condition: halt before main thread can push a task, task lost
        void Halt()
            {
            Lock(&mutex);
            while ( front != nullptr )
                {
                Wait(&cv, &mutex);
                }
            halted.store( true );
            Broadcast(&cv);
            Unlock(&mutex);
            }

        bool IsHalted( )
            {
            return halted.load();
            }

    private:
        QueueItem *front;
        QueueItem *back;

        mutex_t mutex;
        cv_t cv;

        std::atomic<bool> halted;
    };