// Simple hash table template.

// Nicole Hamilton  nham@umich.edu

#pragma once

#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include "string.h"
#include "params.h"
#define INITIAL_SIZE 5
using namespace std;


// You may add additional members or helper functions.


// Compare C-strings, return true if they are the same.

bool CompareEqual( const char *L, const char *R );

bool CompareEqual(const String &L,const String &R); /*{
    cout << "In static ocmpare qequal" << endl;
    cout << "comparing: " << L.cstr() << " and " << R.cstr() << endl;
    bool ret = strcmp(L.cstr(),R.cstr()) == 0;
    cout << "Returning with : " << ret << endl;
    return ret;
}*/

uint32_t fnvHash( const char *data, size_t length );


template<typename Key, typename Value>
class Tuple {
public:
    Key key;
    Value value;

    Tuple(const Key &k, const Value v) : key(k), value(v) {
    }
};


template<typename Key, typename Value>
class Bucket {
public:
    Bucket *next;
    uint32_t hashValue;
    Tuple<Key, Value> tuple;

    Bucket(const Key &k, uint32_t h, const Value v) :
            tuple(k, v), next(nullptr), hashValue(h) {
    }
};


template<typename Key, typename Value>
class HashTable {
private:

    // Your code here.

    Bucket< Key, Value > **buckets;
    size_t numberOfBuckets, numberOfEntries;

    friend class Iterator;

    friend class HashBlob;

public:

    Tuple< Key, Value > *Find( const Key &k, const Value initialValue ) 
        {
            //cout << "IN FIND with key " << (String)k << endl;
        // Search for the key k and return a pointer to the
        // ( key, value ) entry.  If the key is not already
        // in the hash, add it with the initial value.

        // Your code here.
            // TODO: Does it matter if length includes nullterm;
        uint32_t hashValue = fnvHash( ((String)k).cstr(), strlen( ((String)k).cstr()  ));
        Bucket< Key, Value > *curr = buckets[ hashValue % numberOfBuckets ];
        while ( curr ) 
            {
            if ( CompareEqual( curr->tuple.key, k ) )
                return &curr->tuple;
            if ( !curr->next )
                break;
            else curr = curr->next;
            }

        Bucket< Key, Value > *new_bucket = new Bucket< Key, Value >( k, hashValue, initialValue );
        if ( curr )
            curr->next = new_bucket;
        else 
            buckets[ hashValue % numberOfBuckets ] = new_bucket;
        numberOfEntries++;

        return &new_bucket->tuple;
        }

    Tuple<Key, Value> *Find(const Key &k) const {
        // Search for the key k and return a pointer to the
        // ( key, value ) enty.  If the key is not already
        // in the hash, return nullptr.
        // Your code here.
        /*
        const char* str = ((String)k).cstr();
        std::cout << "Got find: " << ((String)k).buffer << endl;
        for(unsigned int i = 0; i < 10; ++i){
            cout << "str[" << i << "]: " << str[i] << " ";
        }
        cout << endl;
        */
        size_t index = fnvHash( ((String)k).cstr(), strlen( ((String)k).cstr() ) ) % numberOfBuckets;
        Bucket< Key, Value > *curr = buckets[ index ];
        while ( curr ) 
            {
            if ( CompareEqual( curr->tuple.key, k ) )
                return &curr->tuple;
            curr = curr->next;
            }
        return nullptr;
    }

    void Optimize(double loading = 1.5) {
        // Modify or rebuild the hash table as you see fit
        // to improve its performance now that you know
        // nothing more is to be added.

        // Your code here.
        HashTable tmp( numberOfEntries / loading );
        tmp.numberOfEntries = numberOfEntries;
        // iterate over indexes
        for ( int i = 0; i < numberOfBuckets; ++i )
            {
            Bucket< Key, Value > *curr = buckets[ i ], *next;
            buckets[ i ] = nullptr;
            while ( curr )
                {
                next = curr->next;
                curr->next = nullptr;
                Bucket< Key, Value > *newLoc = tmp.buckets[ curr->hashValue % tmp.numberOfBuckets ];
                if ( newLoc )
                    {
                    while ( newLoc->next )
                        newLoc = newLoc->next;
                    newLoc->next = curr;
                    }
                else
                    tmp.buckets[ curr->hashValue % tmp.numberOfBuckets ] = curr;
                curr = next;
                }
            }            
        // swap contents
        Bucket< Key, Value > **tmpBuckets = buckets;
        buckets = tmp.buckets;
        tmp.buckets = tmpBuckets;

        size_t tmpNB = numberOfBuckets;
        numberOfBuckets = tmp.numberOfBuckets;
        tmp.numberOfBuckets = tmpNB;
    }


    // Your constructor may take as many default arguments
    // as you like.

    HashTable( size_t initBuckets = INITIAL_SIZE ) 
        : numberOfBuckets( initBuckets ), numberOfEntries( 0 )
        {
        // Your code here.
        buckets = new Bucket< Key, Value > *[ numberOfBuckets ]( );
        }


    ~HashTable( ) 
        {
        // Your code here.
        for ( int i = 0; i < numberOfBuckets; i++ ) 
            {
            Bucket< Key, Value > *curr = buckets[ i ];
            while ( curr ) 
                {
                Bucket< Key, Value > *temp = curr->next;
                delete curr;
                curr = temp;
                }
        }
        delete[] buckets;
    }


    class Iterator {
    private:

        friend class HashTable;

        // Your code here.
        HashTable *table;
        Bucket< Key, Value > *cur_pointer;
        size_t cur_index;

        Iterator( HashTable *table, size_t bucket, Bucket< Key, Value > *b ) 
            {
            // Your code here.
            this->table = table;
            this->cur_index = bucket;
            this->cur_pointer = b;
            }

    public:

        Iterator() : Iterator( nullptr, 0, nullptr ) 
            {
            }

        ~Iterator() 
            {
            }

        Tuple<Key, Value> &operator*( ) 
            {
            // Your code here.
            return cur_pointer->tuple;
            }

        Tuple<Key, Value> *operator->( ) const 
            {
            // Your code here.
            return &cur_pointer->tuple;
            }

        // Prefix ++
        Iterator &operator++( ) 
            {
            // Your code here.
            if ( cur_pointer->next ) 
                cur_pointer = cur_pointer->next;
            else 
                {
                size_t size = table->numberOfBuckets;
                Bucket< Key, Value > **buckets = table->buckets;
                bool if_over = true;
                for ( uint64_t i = cur_index + 1; i < size; i++ ) 
                    {
                    if ( buckets[ i ] ) 
                        {
                        cur_index = i;
                        cur_pointer = buckets[ i ];
                        if_over = false;
                        break;
                        }
                    }
                // no buckets left
                if ( if_over ) 
                    {
                    cur_index = size;
                    cur_pointer = nullptr;
                    }
                }
            return *this;
            }

        // Postfix ++
        Iterator operator++( int ) 
            {
            // Your code here.
            Bucket< Key, Value > *prev_pointer = cur_pointer;
            size_t prev_index = cur_index;
            ++( *this );
            return Iterator( table, prev_index, prev_pointer );
            }

        bool operator==(const Iterator &rhs) const 
            {
            // Your code here.
            return cur_pointer == rhs.cur_pointer;
            }

        bool operator!=(const Iterator &rhs) const 
            {
            // Your code here.
            return !( *this == rhs );
            }
    };

    Iterator begin() 
        {
        // Your code here.
        for ( int i = 0; i < numberOfBuckets; i++ ) 
            if ( buckets[ i ] ) 
                return Iterator( this, i, buckets[ i ] );
        return Iterator( this, 0, nullptr );
        }

    Iterator end() 
        {
        // Your code here.
        return Iterator( this, numberOfBuckets, nullptr );
        }

    const Iterator cbegin( ) const
        {
        return begin( );
        }

    const Iterator cend( ) const
        {
        return end( );
        }

};
