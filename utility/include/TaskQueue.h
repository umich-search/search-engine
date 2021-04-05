#pragma once
#include <atomic>
#include <pthread.h>

struct Task
    {
    private:
        void *arg;
        bool delArgs; // Should the tasked thread free the args from memory?
        std::atomic<bool> *halted; // Check this in infinitely running tasks

    public:
        Task( void* args, bool deleteArgs, std::atomic<bool> *halt ) 
            : arg( args ), delArgs( deleteArgs ), halted( halt ) { }

        bool IsHalted() { return halted->load(); }

        void *GetArgs() { return arg; }
        bool DeleteArgs() { return delArgs; }
    };

struct QueueItem
    {
    Task *task;
    QueueItem *next;
    };

class TaskQueue
    {
    public:
        TaskQueue() 
            {
            halted.store( false );
            front = nullptr;
            back = nullptr;
            pthread_mutex_init(&mutex, nullptr);
            pthread_cond_init(&cv, nullptr);
            }

        ~TaskQueue() 
            {
            while ( front != nullptr )
                {
                QueueItem *temp = front;
                front = front->next;
                delete temp;
                }
            }

        void PushTask( void *args, bool deleteArgs ) 
            { 
            // Create a task
            Task *task = new Task( args, deleteArgs, &halted );
            // Create a place in task queue
            QueueItem *item = new QueueItem;
            item->task = task;
            item->next = nullptr;
            // Add the task to the back of the queue
            pthread_mutex_lock(&mutex);
            if ( front == nullptr )
                front = item;
            if ( back != nullptr )
                back->next = item;
            back = item;
            // Signal an available task for a thread
            pthread_cond_signal(&cv);
            pthread_mutex_unlock(&mutex);
            }

        // Wait for task if necessary. May return nullptr if queue halted.
        Task *PopTask( ) 
            {
            pthread_mutex_lock(&mutex);
            while ( !IsHalted() && (front == nullptr) )
                {
                pthread_cond_wait(&cv, &mutex);
                }
            if ( front == nullptr )
                {
                pthread_mutex_unlock(&mutex);
                return nullptr;
                }
            Task *task = front->task;
            QueueItem* temp = front;
            front = temp->next;
            delete temp;
            pthread_cond_broadcast(&cv);   
            pthread_mutex_unlock(&mutex);
            return task;      
            }

        // Wait until the task queue is empty. 
        void WaitForEmptyQueue() 
            {
            pthread_mutex_lock(&mutex);
            while ( !IsHalted() && ( front != nullptr ) )
                {
                pthread_cond_wait(&cv, &mutex);
                }
            pthread_mutex_unlock(&mutex);
            }

        // Wait until all tasks are complete before halting.
        // Race condition: halt before main thread can push a task, task lost
        void Halt()
            {
            pthread_mutex_lock(&mutex);
            while ( front != nullptr )
                {
                pthread_cond_wait(&cv, &mutex);
                }
            halted.store( true );
            pthread_cond_broadcast(&cv);
            pthread_mutex_unlock(&mutex);
            }

        bool IsHalted( )
            {
            return halted.load();
            }

    private:
        QueueItem *front;
        QueueItem *back;

        pthread_mutex_t mutex;
        pthread_cond_t cv;

        std::atomic<bool> halted;
    };