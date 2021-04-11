#include "CrawlerApp.h"

int main ( int argc, char **argv )
    {
    if ( argc != 2 )
        {
        std::cerr << "Usage: " << argv[0] << " frontierInit(0/1) " << std::endl;
        return 1;
        }
    bool frontierInit = strncmp(argv[1], "1", 1) == 0;
    CrawlerApp app( frontierInit );
    app.Start();
    app.Join();
    return 0;
    }