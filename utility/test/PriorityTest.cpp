#include "PriorityQueue.h"
#include "mString.h"
#include <cstdlib>
#include <iostream>

using namespace std;

class NewString
    {
    String s;

    friend ostream& operator<< ( ostream& , const NewString& );

    public:
        NewString( ) { }

        NewString( const String& ss ) : s( ss ) 
            { 
            }


        bool operator< ( const NewString& other )
            {
            return s.size( ) < other.s.size( );
            }
        
        bool operator<= ( const NewString& other )
            {
            return s.size( ) <= other.s.size( );
            }

        bool operator> ( const NewString& other )
            {
            return !( *this <= other );
            }

        bool operator>= ( const NewString& other )
            {
            return !( *this < other );
            }
    };

ostream& operator<< ( ostream& x, const NewString& y )
    {
    return x << y.s;
    }

int main( )
    {
    cout << "######################################\n\n" << 
        "PriorityTest" << "\n\n######################################\n\n";

    PriorityQueue< int > a;
    PriorityQueue< String > b;
    PriorityQueue< NewString > c;

    for ( int i = 0; i < 20; i++ )
        {
        a.Push( rand( ) % 100 );
        }
    cout << "Priority queue of ints\n";
    a.PrintHeap( );
    cout << "\nHeap sort:\n";
    a.PrintHeapSort( );

    for ( int i = 0; i < 20; i++ )
        {
        // get random length of the input String
        int len = rand( ) % 10;
        String cand;
        // append random characters to the String
        for ( int j = 0; j < len; j++ )
            cand += ( char )'a' + rand( ) % 26;
        b.Push( cand );
        }
    cout << "\nPriority queue of strings by normal order\n";
    b.PrintHeap( );
    cout << "\nHeap sort:\n";
    b.PrintHeapSort( );

    while( !b.empty( ) )
        {
        String bt = b.Top( );
        c.Push( bt );
        b.Pop( );
        }
    cout << "\nPriority queue of strings by length\n";
    c.PrintHeap( );
    cout << "\nHeap sort:\n";
    c.PrintHeapSort( );

    }