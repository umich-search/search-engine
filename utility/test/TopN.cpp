// TopN.cpp
// Nicole Hamilton nham@umich.edu

// Given a hashtable, return a dynamically-allocated array
// of pointers to the top N pairs based on their values.
// Caller is responsible for deleting the array.

// Individual pairs are to be C-strings as keys and size_t's
// as values.

#include "HashTable.h"
#include "TopN.h"
#include<queue>
using namespace std;


using Hash = HashTable<const char *, size_t>;
using Pair = Tuple<const char *, size_t>;
class ComparisonClass {
public:
    bool operator() ( Pair *pair1, Pair *pair2 ) {
        //comparison code here
        return pair1->value > pair2->value;
    }
};

Pair **TopN(Hash *hashtable, int N) {
    // Find the top N pairs based on the values and return
    // as a dynamically-allocated array of pointers.  If there
    // are less than N pairs in the hash, remaining pointers
    // will be null.

    // Your code here.
    //use min heap
    priority_queue< Pair *,vector< Pair * >, ComparisonClass > store;
    for ( HashTable< const char*, size_t >::Iterator iterator = hashtable->begin(); iterator != hashtable->end( ); ++iterator )
        {
        if ( store.size( ) < N )
            store.push( &( *iterator ) );
        else
            {
            if( ( iterator->value ) > ( store.top( )->value ) )
                {
                store.pop();
                store.push( &( *iterator ) );
                }
            }
        }

    Pair ** result = new Pair*[N];
    int i = N - 1;
    while ( !store.empty( ) ) 
        {
        result[i--]=store.top( );
        store.pop();
        }

    return result;
}
