#include <fstream>
#include "Common.h"
#include <string>
#include <cassert>

using namespace std;

const char *seedlist = "top-1m.txt";
const char *seedMs[ 7 ] = 
    {
    "seedlist/seedM1.txt",
    "seedlist/seedM2.txt",
    "seedlist/seedM3.txt",
    "seedlist/seedM4.txt",
    "seedlist/seedM5.txt",
    "seedlist/seedM6.txt",
    "seedlist/seedM7.txt",
    };

int main( )
    {
    ifstream in;
    ofstream out[ 7 ];
    in.open( seedlist );
    for ( int i = 0; i < 7; ++i )
        out[ i ].open( seedMs[ i ] );
    string s;
    while ( in >> s )
        {
        assert( s.back( ) != '\n' );
        size_t idx = fnvHash( s.c_str( ), s.length( ) ) % 7;
        out[ idx ] << s << '\r' 
            << '\n';
        }
    for ( int i = 0; i < 7; ++i )
        out[ i ].close( );
    in.close( );
    }