#include "CrawlerApp.h"

int main ( int argc, char **argv )
    {
    if ( argc != 3 )
        {
        std::cerr << "Usage: " << argv[0] << " machineID(0-13) frontierInit(0/1) " << std::endl;
        return 1;
        }
    size_t machineID = atoi( argv[1] );
    if ( machineID < 0 || machineID > 13 )
        {
        std::cerr << "Invalid machine ID: " << machineID << std::endl;
        return 1;
        }
    bool frontierInit = strncmp( argv[2], "1", 1 ) == 0;
    CrawlerApp app( machineID, frontierInit );
    app.Start();
    app.Join();
    return 0;
    }