#include <iostream>
#include "Frontier.h"
#include "Concurrency.h"
#include <fstream>
#include <iostream>

using namespace std;

struct p
    {
    Frontier *x;
    int y;
    };

void *func( void *x )
    {
    Frontier *ft = (( p * )x)->x;
    int k = (( p * )x)->y;
    for ( int i = 0; i < 18; ++i )
        std::cout << "id = " << k << ' ' << ft->PopUrl( ) << std::endl;
    return nullptr;
    }

int main( int argc, char *argv[] )
    {
    const char *nm = "./seen.txt", *dbg = "./crawler/test/seen.txt";
    if ( argc != 2 )
        exit( 1 );
    Frontier a( argv[1], 3, 2);

    pthread_t *tp = new pthread_t[ 3 ];
    p *ag = new p[ 3 ];
    for ( int i = 0; i < 3; ++i)
        {
        ag[ i ].x = &a;
        ag[ i ].y = i;
        pthread_create( tp + i, nullptr, func, ( void * )( ag + i ) );
        }

    ifstream fin;
    fin.open( nm );
    string s;
    int i = 0;
    while ( fin >> s && i < 100 )
        {
        ++i;
        Link l( s.c_str( ) );
        a.PushUrl( l );
        }

    for ( int i = 0; i < 3; ++i )
        pthread_join( tp[ i ], nullptr );
    delete [] tp;
    delete [] ag;
    return 0;
    }