#pragma once
#include "Concurrency.h"
#include <pthread.h>

// ----- Queue.h
// Thread-safe queue
// Push: add a value to the queue.
// Pop: remove a value from the queue. wait if queue is empty.
// Block: block the queue from push/pop.

template <typename T>
class Queue
    {
    public:
        Queue() 
            {
            blocked = false;
            front = nullptr;
            back = nullptr;
            size = 0;
            MutexInit(&mutex, nullptr);
            CvInit(&cv, nullptr);
            }

        ~Queue() 
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

        bool Push( const T &val ) 
            { 
            Lock(&mutex);
            if ( blocked )
                {
                Unlock(&mutex);
                return false;
                }
            QueueItem *item = new QueueItem( val, nullptr );            
            if ( front == nullptr )
                front = item;
            if ( back != nullptr )
                back->next = item;
            back = item;
            ++size;
            Signal(&cv);
            Unlock(&mutex);
            return true;
            }

        bool Pop( T &val ) 
            {
            Lock(&mutex);
            while ( !blocked && ( front == nullptr ) )
                {
                Wait(&cv, &mutex);
                }
            if ( front == nullptr )
                {
                Unlock(&mutex);
                return false;
                }
            val = front->val;
            QueueItem* temp = front;
            front = temp->next;
            delete temp;
            --size;
            Broadcast(&cv);   
            Unlock(&mutex);
            return true;      
            }

        size_t Size()
            {
            return size;
            }

        void WaitUntilEmpty()
            {
            Lock(&mutex);
            while ( !blocked && ( front == nullptr ))
                {
                Wait(&cv, &mutex);
                }
            Unlock(&mutex);
            }

        void Block()
            {
            Lock(&mutex);
            blocked = true;
            Broadcast(&cv);
            Unlock(&mutex);
            }
        
        void Unblock()
            {
            Lock(&mutex);
            blocked = false;
            Broadcast(&cv);
            Unlock(&mutex);
            }

    private:
        struct QueueItem
            {
            QueueItem( T val, QueueItem *next )
                : val( val ), next( next ) { }

            T val;
            QueueItem *next;
            };
            
        QueueItem *front;
        QueueItem *back;
        size_t size;

        mutex_t mutex;
        cv_t cv;

        bool blocked;
    };