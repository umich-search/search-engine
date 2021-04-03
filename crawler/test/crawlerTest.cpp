#include "Crawler.h"

// test robot parsing
void testRobot( Crawler *cp )
    {
    static const char *robotUrl = "https://www.google.com";
    cp->parseRobot( robotUrl );
    }

int main( )
    {
    Frontier ft( "ft", 3, 2 );
    FileBloomfilter bf( "bf", 100, 0.00001 );
    Crawler c;
    c.setParameters( 0, &ft, &bf );
    testRobot( &c );
    std::cout << "try to finish\n";
    }