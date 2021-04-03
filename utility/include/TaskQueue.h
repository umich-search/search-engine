#pragma once
#include <atomic>
#include <pthread.h>

struct Task
    {
    void *args;
    Task *next;
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
                Task *temp = front;
                front = front->next;
                delete temp;
                }
            }

        void PushTask( void *args ) 
            { 
            // Create a task
            Task *newTask = new Task;    
            newTask->args = args;
            newTask->next = nullptr;
            // Add the task to the queue
            pthread_mutex_lock(&mutex);
            if ( front == nullptr )
                front = newTask;
            if ( back != nullptr )
                back->next = newTask;
            back = newTask;
            // Signal an available task for a thread
            pthread_cond_signal(&cv);
            pthread_mutex_unlock(&mutex);
            }

        // Wait for task if necessary. May return nullptr if queue halted.
        void *PopTask( ) 
            {
            void *args;
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
            Task* temp = front;
            args = temp->args;
            front = temp->next;
            delete temp;
            pthread_cond_broadcast(&cv);   
            pthread_mutex_unlock(&mutex);
            return args;      
            }

        // Wait until the task queue is empty. 
        void WaitForEmptyQueue() 
            {
            pthread_mutex_lock(&mutex);
            while ( !IsHalted() && front != nullptr )
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
        Task *front;
        Task *back;

        pthread_mutex_t mutex;
        pthread_cond_t cv;

        std::atomic<bool> halted;
    };