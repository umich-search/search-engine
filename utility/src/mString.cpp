#include "mString.h"

std::ostream &operator<<(std::ostream &os, const String &s) {
    for (size_t i = 0; i < s.size(); i++) {
        os << *(s.begin() + i);
    }
    return os;
}

std::istream &operator>>(std::istream& is, String& s)
    {
    std::string a;
    is >> a;
    s = String( a.c_str( ), a.length( ) );
    return is;
    }

String ltos( int x )
    {
    String s, res;
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
    for ( size_t i = s.size( ); i > 0; --i )
        res += s[ i - 1 ];
    return res;
    }