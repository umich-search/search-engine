#include <iostream>
#include "RankerSender.h"

static const char * managerIP = "34.75.224.47";
static const int port = 8888;

int main ( int argc, char **argv )
    {
    RankServer server( managerIP, port, port );
    server.Start();
    }