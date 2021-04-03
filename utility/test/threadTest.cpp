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
    void DoTask( void *args ) override
        {
            String *url = (String *)args;
            std::cout << "Thread (" << GetID() << "): " << url->cstr() << std::endl;
            //int test = 1 / 0;
            return;
        }
    };

int main(int argc, char **argv )
    {
    TaskQueue taskQueue;
    vector<Test> tests(1000);
    bool startup = true;
    for ( size_t i = 0; i < tests.size(); ++i )
        {
            startup = startup && tests[i].Start( i, &taskQueue );
        }
    std::cout << "All threads started: " << startup << std::endl;
    String task1 = "task1";
    String task2 = "task2";
    String task3 = "task3";
    taskQueue.PushTask((void *) &task1);
    taskQueue.PushTask((void *) &task2);
    taskQueue.PushTask((void *) &task3);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    taskQueue.Halt();
    for ( size_t i = 0; i < tests.size(); ++i )
        {
            tests[i].Join();
        }
    }