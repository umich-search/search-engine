#include "SmartPointer.h"
#include <cassert>

int main( )
    {
    SharedPointer< int > a( nullptr ), b( new int{ 3 } ),
        c( b ), d( [](){ return SharedPointer< int >( new int { 4 } ); }() );
    assert( !a );
    assert( (*b + 1 ) == 4 );
    assert( b == c );
    assert( *c == 3 );
    assert( *c.get() == 3 );
    assert( b.numCount() == 2 );
    c = d;
    assert( b.numCount() == 1 );
    assert( *c = 4 );
    assert( d.numCount() == 2 );
    c = new int { 3 };
    d = new int { 0 };
    return 0;
    }