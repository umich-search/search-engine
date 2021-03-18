#include "../DiskQueue.h"
#include <iostream>

const char DNAME[] = "./dq";

int main( )
    {
    DiskQueue a( DNAME );
    a.PrintStatus( );

    String google = "http://google.com\n",
            baidu = "https://baidu.com\n",
            facebook = "https://facebook.com\n";
    a.PushBack( google );
    std::cout << "Sequential Pops: " << a.PopFront( ) << a.PopFront( ) << a.PopFront( ) << std::endl;
    }