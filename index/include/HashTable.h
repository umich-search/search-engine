#pragma once
#include "Common.h"
#include <cstring>
// Number of buckets in hashtable during construction
#define INITIAL_HASHTABLE_SIZE 30000
#define WRITE_TO_DISK true
#define USE_CHUNK_LIMIT true
typedef uint64_t fnvHash_t;


// Compare C-strings, return true if they are the same.

//bool CompareEqual( const char *L, const char *R );

//bool CompareEqual(const String &L,const String &R);

//fnvHash_t fnvHash( const char *data, size_t length );



template<typename Key, typename Value>
class Tuple {
public:
    Key key;
    Value value;

    Tuple(const Key &k, const Value v) : key(k), value(v) {
    }
};

template<typename Key, typename Value>
class Tuple<Key,Value*> {
public:
    Key key;
    Value* value;

    Tuple(const Key &k, Value* v) : key(k), value(v) {
    }
    
    ~Tuple() {
        if(WRITE_TO_DISK){
            delete value;
        }
    }
};


template<typename Key, typename Value>
class Bucket {
public:
    Bucket *next;
    fnvHash_t hashValue;
    Tuple<Key, Value> tuple;

    Bucket(const Key &k, fnvHash_t h, const Value v) :
            tuple(k, v), next(nullptr), hashValue(h) {
    }
};


template<typename Key, typename Value>
class HashTable {
private:


    Bucket< Key, Value > **buckets;
    size_t numberOfBuckets, numberOfEntries;

    friend class Iterator;

    friend class HashBlob;

public:

    Tuple< Key, Value > *Find( const Key &k, const Value initialValue ) 
        {
        // Search for the key k and return a pointer to the
        // ( key, value ) entry.  If the key is not already
        // in the hash, add it with the initial value.

        fnvHash_t hashValue = fnvHash( ((String)k).cstr(), strlen( ((String)k).cstr()  ));
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

        HashTable tmp( numberOfEntries * loading );
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


    HashTable( size_t initBuckets = INITIAL_HASHTABLE_SIZE )
        : numberOfBuckets( initBuckets ), numberOfEntries( 0 )
        {
        buckets = new Bucket< Key, Value > *[ numberOfBuckets ]( );
        }


    ~HashTable( ) 
        {
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

        HashTable *table;
        Bucket< Key, Value > *cur_pointer;
        size_t cur_index;

        Iterator( HashTable *table, size_t bucket, Bucket< Key, Value > *b ) 
            {
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
            return cur_pointer->tuple;
            }

        Tuple<Key, Value> *operator->( ) const 
            {
            return &cur_pointer->tuple;
            }

        // Prefix ++
        Iterator &operator++( ) 
            {
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
            Bucket< Key, Value > *prev_pointer = cur_pointer;
            size_t prev_index = cur_index;
            ++( *this );
            return Iterator( table, prev_index, prev_pointer );
            }

        bool operator==(const Iterator &rhs) const 
            {
            return cur_pointer == rhs.cur_pointer;
            }

        bool operator!=(const Iterator &rhs) const 
            {
            return !( *this == rhs );
            }
    };

    Iterator begin() 
        {
        for ( int i = 0; i < numberOfBuckets; i++ )
            if ( buckets[ i ] ) 
                return Iterator( this, i, buckets[ i ] );
        return Iterator( this, 0, nullptr );
        }

    Iterator end() 
        {
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
