#include <iostream>
#include "RankServer.h"

int main ( int argc, char **argv )
    {
    if ( argc != 2 )
        {
        std::cerr << "Usage: " << argv[0] << " port " << std::endl;
        }
    size_t port = atoi( argv[1] );
    RankServer server( port );
    server.Start();
    }