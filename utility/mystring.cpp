#include "mystring.h"

std::ostream &operator<<(std::ostream &os, const String &s) {
    for (size_t i = 0; i < s.size(); i++) {
        os << *(s.begin() + i);
    }
    return os;
}

String ltos( int x )
    {
    String s;
    if ( x == 0 )
        {
        s += '0';
        return s;
        }
    while ( x > 0 ) 
        {
        s += String( ( char )( '0' + x % 10 ) );
        x /= 10;
        }
    return s;
    }