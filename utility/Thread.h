#pragma once
#include <pthread.h>
#include <atomic>

class Thread
    {
    pthread_t thread;
    std::atomic<bool> dead;

    public:
        Thread( )
        {
            dead.store( false );
        }

        ~Thread( )
        {
            Kill();
            Join(); // thread may take some time to finish, so wait to destruct
        }

        bool Start()
        {
            return pthread_create( &thread, nullptr, ThreadStart, this ) == 0;
        }

        void Join()
        {
            pthread_join( thread, nullptr );
        }

        void Kill()
        {
            dead.store( true );
        }

        // Check this condition in an infinitely running thread
        bool isDead()
        {
            return dead.load();
        }

    protected:
        // Override this function with what the thread should run
        virtual void Work() = 0;

    private:
        static void * ThreadStart( void * context ) 
        {
            ( (Thread *) context )->Work();
            return nullptr;
        }
    };