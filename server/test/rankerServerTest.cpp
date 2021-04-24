#include <iostream>
#include "RankerSender.h"
#include "Results.h"

const char *machineDir[ ] = 
    {
    "m1/", "m2/", "m3/",
    };

size_t port[ ] = { 8000, 8001, 8002, };

int main( int argc, char *argv[ ] )
    {
    if ( argc != 2 )
        {
        std::cout << "Usage: ./rankerServerTest [machine id].\n";
        return -1;
        }
    int myport = port[ atoi( argv[ 1 ] ) ];
    RankServer server( "127.0.0.1", myport, 8888 );
    try
        {
        server.Start( );
        }
    catch( std::string& e )
        {
        std::cout << e << std::endl;
        }
    
    }