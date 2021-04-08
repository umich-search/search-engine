#include <iostream>
#include "Frontier.h"
#include "Concurrency.h"
#include "DiskQueue.h"
#include <fstream>
#include <iostream>

using namespace std;

struct p
    {
    Frontier *x;
    int y;
    };

mutex_t m;
int numt = 0, numwt = 1;
const char *nm = "./seen.txt", *dbg = "./crawler/test/seen.txt";


void printWithLock( int id, String s )
    {
    CriticalSection ct( &m );
    std::cout << "id = " << id << ' ' << s << std::endl;
    }

void *func( void *x )
    {
    Frontier *ft = (( p * )x)->x;
    int k = (( p * )x)->y;
    for ( int i = 0; i < 100; ++i )
        {
        String st = ft->PopUrl( );
        printWithLock( k, st );
        }
    return nullptr;
    }

void *writeFunc( void *x )
    {
    Frontier *ft = ( Frontier *) x;
    ifstream fin;
    fin.open( nm );
    string s;
    int i = 0;
    while ( fin >> s && i < 1 )
        {
        ++i;
        Link l( s.c_str( ) );
        ft->PushUrl( l );
        }
    fin.close( );
    return nullptr;
    }

void init( Frontier *f, const char *seedFile )
    {
    f->FrontierInit( seedFile );
    }

int main( int argc, char *argv[] )
    {
    if ( argc != 3 )
        exit( 1 );
    Frontier a( argv[1], 20, 15);
    init( &a, argv[2] );
    // MutexInit( &m, nullptr );

    // pthread_t *tp = new pthread_t[ numt ],
    //     *wtp = new pthread_t[ numwt ];
    // p *ag = new p[ numt ];
    // for ( int i = 0; i < numt; ++i)
    //     {
    //     ag[ i ].x = &a;
    //     ag[ i ].y = i;
    //     pthread_create( tp + i, nullptr, func, ( void * )( ag + i ) );
    //     }
    // for ( int i = 0; i < numwt; ++i )
    //     {
    //     pthread_create( wtp + i, nullptr, writeFunc, ( void * )( &a ) );
    //     }
    

    // for ( int i = 0; i < numt; ++i )
    //     pthread_join( tp[ i ], nullptr );
    // for ( int i = 0; i < numwt; ++i )
    //     pthread_join( wtp[ i ], nullptr );

    // MutexDestroy( &m );
    // delete [] tp;
    // delete [] wtp;
    // delete [] ag;
    return 0;
    }