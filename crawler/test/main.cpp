#include "CrawlerApp.h"

using namespace std;

int main( )
    {
    CrawlerApp::Parameters param;
    param.numCrawlThreads = 10;
    param.numListenThreads = 3;
    param.numSendThreads = 3;
    param.frontierDir = "frontier";
    param.numDiskQueue = 8;
    param.pqSize = 5;
    param.filterDir = "filter";
    param.numObjects = 1000000;
    param.fpRate = 0.0001;

    CrawlerApp web( param );

    }