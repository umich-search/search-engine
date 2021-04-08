#include "SmartPointer.h"
#include <cassert>
#include <utility>
#include <pthread.h>

struct T
{
    int a;
    char *c;
};


using sharedpointer = SharedPointer< T >;

void *f1( void *p )
    {
    return nullptr;
    }

void *f2( void *p )
    {
    return nullptr;
    }

void *f3( void *p )
    {
    return nullptr;
    }


int main( )
    {

    return 0;
    }