#include <fstream>
#include "Common.h"
#include <string>
#include <cassert>

using namespace std;

const char *seedlist = "top-1m.txt";
const char *seedMs[ 14 ] = 
    {
    "seedlist/seedM0-0.txt",
    "seedlist/seedM0-1.txt",
    "seedlist/seedM1-0.txt",
    "seedlist/seedM1-1.txt",
    "seedlist/seedM2-0.txt",
    "seedlist/seedM2-1.txt",
    "seedlist/seedM3-0.txt",
    "seedlist/seedM3-1.txt",
    "seedlist/seedM4-0.txt",
    "seedlist/seedM4-1.txt",
    "seedlist/seedM5-0.txt",
    "seedlist/seedM5-1.txt",
    "seedlist/seedM6-0.txt",
    "seedlist/seedM6-1.txt",
    };

int main( )
    {
    ifstream in;
    ofstream out[ 14 ];
    in.open( seedlist );
    for ( int i = 0; i < 14; ++i )
        out[ i ].open( seedMs[ i ] );
    string s;
    while ( in >> s )
        {
        assert( s.back( ) != '\n' );
        size_t idx = fnvHash( s.c_str( ), s.length( ) ) % 14;
        out[ idx ] << s << '\r' 
            << '\n';
        }
    for ( int i = 0; i < 14; ++i )
        out[ i ].close( );
    in.close( );
    }