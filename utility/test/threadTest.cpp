#include "../include/Thread.h"
#include "../include/Vector.h"
#include "../include/mString.h"
#include "../include/TaskQueue.h"
#include <iostream>
#include <chrono>
#include <thread>

class Test : public Thread 
    {
public:
private:
    void DoTask( Task *task ) override
        {
        std::string *args = (std::string *) task->GetArgs();
        Print(*args);
        //int test = 1 / 0;
        if ( task->DeleteArgs() )
            delete args;
        return;
        }
    };

int main(int argc, char **argv )
    {
    TaskQueue taskQueue;
    pthread_mutex_t printMutex;
    pthread_mutex_init(&printMutex, nullptr);
    vector<Test> threads(100);
    // Test thread creation
    bool startup = true;
    for ( size_t i = 0; i < threads.size(); ++i )
        {
        startup = startup && threads[i].Start( i, &taskQueue, &printMutex );
        }
    std::cout << "All threads started: " << startup << std::endl;
    // Test stack args (no dynamic memory)
    std::string args("Stack");
    for ( size_t i = 0; i < 1000; ++i )
        {
        taskQueue.PushTask((void *)&args, false );
        }
    taskQueue.WaitForEmptyQueue();
    // Test heap args (dynamic memory)
    for ( size_t i = 0; i < 1000; ++i )
        {
        std::string* args = new std::string("Heap");
        taskQueue.PushTask((void *)args, true );
        }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    taskQueue.Halt();
    for ( size_t i = 0; i < threads.size(); ++i )
        {
        threads[i].Join();
        }
    }