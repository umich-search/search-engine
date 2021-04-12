#include "CrawlerManager.h"
#include "CrawlerApp.h"
#include "mString.h"
#include <iostream>

using namespace std;

int main( )
    {
    CrawlerApp c( 0, false );
    // c.Start() will have crawler blocked waiting for enough
    // url inserted into pq with listen manager listening and 
    // send manager waiting(blocked) for tasks to send 
    c.Start( );
    // c.Stop();
    }