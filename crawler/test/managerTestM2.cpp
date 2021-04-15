#include "CrawlerManager.h"
#include "CrawlerApp.h"
#include "mString.h"
#include <iostream>

using namespace std;

int main( )
    {
    try
    {
        CrawlerApp c( 0, false, 8888, 8000 );
        c.Start( );
        c.Join( );
    } catch( const char *ct )
    {
    std::cout << "Exception " <<  ct << std::endl;
    return 1;
    }
    // c.Start() will have crawler blocked waiting for enough
    // url inserted into pq with listen manager listening and 
    // send manager waiting(blocked) for tasks to send 

    }