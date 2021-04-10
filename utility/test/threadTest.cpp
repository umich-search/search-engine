//#include "../include/Vector.h" // compilation error
//#include "../include/mString.h" // runtime error
#include "../include/ThreadPool.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

class Test : public ThreadPool
    {
public:
    Test( Init init )
        : ThreadPool( init ) { }
private:
    void DoTask( Task task, size_t threadID ) override
        {
        std::string *args = (std::string *) task.args;
        Print(*args, threadID);
        // int test = 1 / 0;
        if ( task.deleteArgs )
            delete args;
        return;
        }
    };

int main(int argc, char **argv )
    {
    pthread_mutex_t printMutex;
    pthread_mutex_init(&printMutex, nullptr);

    ThreadPool::Init init;
    init.name = "Test";
    init.numThreads = 10;
    init.printMutex = &printMutex;
    Test test( init );
    // Test thread creation
    test.Start();
    std::cout << "Thread pool alive: " << test.IsAlive() << std::endl;
    if ( !test.IsAlive() )
        return 1;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    vector<std::string> argsVec(1000);
    for ( size_t i = 0; i < argsVec.size(); ++i )
        {
        // Test stack args (no dynamic memory)
        argsVec[i] = "Stack " + std::to_string(i);
        test.PushTask((void *)&argsVec[i], false );
        // Test heap args (dynamic memory)
     o   std::string* args = new std::string("Heap " + std::to_string(i));
        test.PushTask((void *)args, true );
        }
    test.Stop();
    }