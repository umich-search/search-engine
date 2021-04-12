#include "ThreadPool.h"

ThreadPool::ThreadPool( ThreadPool::Init init )
    : name( init.name ), threads( init.numThreads ), printMutex( init.printMutex ), 
    machineID( init.machineID), alive( false ), threadID( 0 )
    {
    MutexInit( &mutex, nullptr );
    }

ThreadPool::~ThreadPool( )
    {
    Stop();
    MutexDestroy(&mutex);
    }

ThreadPool& ThreadPool::operator= ( const ThreadPool& other )
    {
    CriticalSection s( &mutex );
    name = other.name;
    printMutex = other.printMutex;
    threads = vector< pthread_t >( other.threads.size() );
    return *this;
    }

void ThreadPool::Start()
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

void ThreadPool::Stop()
    {
    CriticalSection s( &mutex );
    if ( !alive )
        return;
    taskQueue.WaitUntilEmpty();
    alive = false;
    taskQueue.Block();
    Join();
    }

void ThreadPool::Join()
    {
    for ( size_t i = 0; i < threads.size(); ++i )
        pthread_join( threads[i], NULL );
    }

bool ThreadPool::PushTask( void *args, bool deleteArgs )
    {
    CriticalSection s( &mutex );
    if ( !alive )
        return false;
    Task task;
    task.args = args;
    task.deleteArgs = deleteArgs;
    return taskQueue.Push( task );
    }

bool ThreadPool::IsAlive( )
    {
    return alive;
    }

void ThreadPool::Print( String output, size_t threadID )
    {
    Lock(printMutex);
    std::cout << "M:" << machineID << " Thread(" << name.cstr() << ":" << threadID << "): " 
                << output << std::endl;
    Unlock(printMutex);
    }

void ThreadPool::Work( )
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
                Print(String("Exception"), ID );
                }
            }
        }
    Print("Exited", ID);
    }