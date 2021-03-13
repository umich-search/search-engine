// Concurrency
// This header file defines alias for many type names and function names
// in pthread.h in order to simplify the spelling. It also provides other
// concurrency utilities including RAII, ReaderLock, and WriterLock.

#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <cassert>

typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cv_t;

auto MutexInit = pthread_mutex_init;
auto CvInit = pthread_cond_init;
auto Lock = pthread_mutex_lock;
auto Unlock = pthread_mutex_unlock;
auto Wait = pthread_cond_wait;
auto Signal = pthread_cond_signal;
auto Broadcast = pthread_cond_broadcast;
auto MutexDestroy = pthread_mutex_destroy;
auto CvDestroy = pthread_cond_destroy;

// Slides versin of RAII
class CriticalSection
    {
    private:
        bool locked;
        mutex_t *mutex;

        void Take( )
            {
            assert( !locked );
            Lock( mutex );
            locked = true;
            }

        void Release( )
            {
            assert( locked );
            locked = false;
            Unlock( mutex );
            }

    public:
        CriticalSection( mutex_t *m ) 
            : locked( false ), mutex( m )
            {
            Take( );
            }

        ~CriticalSection( )
            {
            if ( locked )
                Release( );
            }        
    };

class ReadWriteLock
    {
    private:
        int numReader, numWriter;
        mutex_t mutex;
        cv_t readCv, writeCv;

    public:
        ReadWriteLock( ) : numReader( 0 ), numWriter( 0 )
            {
            MutexInit( &mutex, nullptr );
            CvInit( &readCv, nullptr );
            CvInit( &writeCv, nullptr );
            }

        ~ReadWriteLock( )
            {
            MutexDestroy( &mutex );
            CvDestroy( &readCv );
            CvDestroy( &writeCv );
            }

        void ReaderStart( )
            {
            Lock( &mutex );
            while ( numWriter > 0 )
                Wait( &readCv, &mutex );
            ++numReader;
            Unlock( &mutex );
            }

        void ReaderFinish( )
            {
            Lock( &mutex );
            assert( numReader > 0 );
            --numReader;
            if ( numReader == 0 )
                Signal( &writeCv );
            Unlock( &mutex );
            }

        void WriterStart( )
            {
            Lock( &mutex );
            while ( numReader > 0 || numWriter > 0 )
                Wait( &writeCv, &mutex );
            ++numWriter;
            Unlock( &mutex );
            }

        void WriterFinish( )
            {
            Lock( &mutex );
            assert( numWriter == 1 );
            --numWriter;
            Broadcast( &readCv );
            Signal( &writeCv );
            Unlock( &mutex );
            }
    };