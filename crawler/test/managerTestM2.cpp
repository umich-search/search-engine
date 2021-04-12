#include "CrawlerManager.h"
#include "CrawlerApp.h"
#include "mString.h"
#include <iostream>

using namespace std;

int main( )
    {
    CrawlerApp c( 1, true );
    // c.Start() will crawl the frontier, getting urls down
    // from the internet and sending appropriate ones to
    // machine 1. 
    c.Start( );
    // c.Stop();
    }