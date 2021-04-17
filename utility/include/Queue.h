#pragma once
#include "Concurrency.h"
#include <pthread.h>

// ----- Queue.h
// Normal queue and Thread-safe queue
// ----- Thread-safe queue
// Push: add a value to the queue.
// Pop: remove a value from the queue. wait if queue is empty.
// Block: block the queue from push/pop. awaken threads that are waiting on queue.

template <typename T>
class Queue
    {
    public:
        Queue() : front( nullptr ), back( nullptr ), size( 0 ) { }

        ~Queue()
            {
            while ( front != nullptr )
                {
                QueueItem *temp = front;
                front = front->next;
                delete temp;
                }
            }

        void Push( T val )
            {
            QueueItem *item = new QueueItem( val, nullptr );            
            if ( front == nullptr )
                front = item;
            if ( back != nullptr )
                back->next = item;
            back = item;
            ++size;
            }

        bool Pop( T &val )
            {
            if ( front == nullptr )
                return false;
            val = front->val;
            QueueItem* temp = front;
            front = temp->next;
            delete temp;
            if ( front == nullptr )
                back = nullptr;
            --size;
            return true;
            }

        bool Empty( )
            {
            return size == 0;
            }
        
        bool Size( )
            {
            return size;
            }

    private:
        struct QueueItem
            {
            QueueItem( T val, QueueItem *next )
                : val( val ), next( next ) { }

            T val;
            QueueItem *next;
            };
        size_t size;
        QueueItem *front;
        QueueItem *back;
    };

template <typename T>
class ThreadSafeQueue
    {
    public:
        ThreadSafeQueue() 
            {
            blocked = false;
            MutexInit(&mutex, nullptr);
            CvInit(&cv, nullptr);
            }

        ~ThreadSafeQueue() 
            {
            MutexDestroy(&mutex);
            CvDestroy(&cv);
            }

        bool Push( T val ) 
            { 
            CriticalSection s(&mutex);
            if ( blocked || queue.Size() > 1000 )
                return false;
            queue.Push( val );
            Signal(&cv);
            return true;
            }

        bool Pop( T &val ) 
            {
            Lock(&mutex);
            while ( !blocked && queue.Empty() )
                Wait(&cv, &mutex);
            bool ret = queue.Pop( val );
            Broadcast(&cv);
            Unlock(&mutex); 
            return ret;      
            }

        void WaitUntilEmpty()
            {
            Lock(&mutex);
            while ( !blocked && !queue.Empty() )
                {
                Wait(&cv, &mutex);
                }
            Unlock(&mutex);
            }

        void Block()
            {
            CriticalSection s(&mutex);
            blocked = true;
            Broadcast(&cv);
            }
        
        void Unblock()
            {
            CriticalSection s(&mutex);
            blocked = false;
            Broadcast(&cv);
            }

    private:
        Queue<T> queue;
        mutex_t mutex;
        cv_t cv;
        bool blocked;
    };