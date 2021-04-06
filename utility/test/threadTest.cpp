//#include "../include/Vector.h" // compilation error
//#include "../include/mString.h" // runtime error
#include "../include/Thread.h"
#include "../include/TaskQueue.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

const int NUM_THREADS = 10;

class Test : public Thread 
    {
public:
    Test( const Thread::Init &init )
        : Thread( init ) { }
private:
    void DoTask( TaskQueue::Task *task ) override
        {
        std::string *args = (std::string *) task->args;
        Print(*args);
        // int test = 1 / 0;
        if ( task->deleteArgs )
            delete args;
        return;
        }
    };

int main(int argc, char **argv )
    {
    TaskQueue taskQueue;
    pthread_mutex_t printMutex;
    pthread_mutex_init(&printMutex, nullptr);

    std::vector<Test> threads;
    Thread::Init init;
    init.ID = 0;
    init.printMutex = &printMutex;
    init.taskQueue = &taskQueue;
    for ( init.ID = 0; init.ID < NUM_THREADS; ++init.ID )
        {
        threads.push_back( Test( init ) );
        }
    // Test thread creation
    bool startup = true;
    for ( size_t i = 0; i < threads.size(); ++i )
        {
        startup = startup && threads[i].Start();
        }
    std::cout << "All threads started: " << startup << std::endl;

    // Test stack args (no dynamic memory)
    std::string args("Stack");
    for ( size_t i = 0; i < 10000; ++i )
        {
        taskQueue.PushTask((void *)&args, false );
        }
    taskQueue.WaitForEmptyQueue();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Test heap args (dynamic memory)
    for ( size_t i = 0; i < 10000; ++i )
        {
        std::string* args = new std::string("Heap");
        taskQueue.PushTask((void *)args, true );
        }

    taskQueue.Halt();
    for ( size_t i = 0; i < threads.size(); ++i )
        {
        threads[i].Join();
        }
    }