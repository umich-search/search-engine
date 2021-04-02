#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include "Bitmap.h"
#include <cmath>
#include "mString.h"
#include <string.h>
#include <openssl/md5.h>
using namespace std;

class Bloomfilter {
public:
    Bloomfilter(int num_objects, double false_positive_rate) {
        //Determine the size of bits of our data vector, and resize
        size_t vecSize = 0 - num_objects * log( false_positive_rate ) / ( log(2) * log(2) );
        container.resize(vecSize);
        //Determine number of hash functions to use
        numHash = vecSize / num_objects * log(2);
    }

    void insert(const String& s) {
        //Hash the string into two unique hashes
        std::pair<uint64_t, uint64_t> myHash = hash(s);
        uint64_t first = myHash.first;
        uint64_t second = myHash.second;
        String firstStr = String(ltos(first).cstr());
        String secondStr = String(ltos(second).cstr());
        //Use double hashing to get unique bit, and repeat for each hash function
        for ( size_t i = 0; i < numHash; ++i )
            {
            uint64_t firstHash, secondHash;
            unsigned char firstBuffer[ MD5_DIGEST_LENGTH ], secondBuffer[ MD5_DIGEST_LENGTH ];
            MD5( (const unsigned char *)firstStr.cstr(), firstStr.size(), firstBuffer); //TODO: change datum.size
            MD5( (const unsigned char *)secondStr.cstr(), secondStr.size(), secondBuffer);
            memcpy( &firstHash, firstBuffer, 8 );
            memcpy( &secondHash, secondBuffer, 8 );
            size_t ind = (firstHash + i * secondHash) % container.size();
            container.set(ind);
            }
    }

    bool contains(const String& s) {
        //Hash the string into two unqiue hashes
        std::pair<uint64_t, uint64_t> myHash = hash(s);
        uint64_t first = myHash.first;
        uint64_t second = myHash.second;
        String firstStr = String(ltos(first).cstr());
        String secondStr = String(ltos(second).cstr());
        //Use double hashing to get unique bit, and repeat for each hash function
        //If bit is false, we know for certain this unique string has not been inserted
        for ( size_t i = 0; i < numHash; ++i )
            {
            uint64_t firstHash, secondHash;
            unsigned char firstBuffer[ MD5_DIGEST_LENGTH ], secondBuffer[ MD5_DIGEST_LENGTH ];
            MD5( (const unsigned char *)firstStr.cstr(), firstStr.size(), firstBuffer); //TODO: change datum.size
            MD5( (const unsigned char *)secondStr.cstr(), secondStr.size(), secondBuffer);
            memcpy( &firstHash, firstBuffer, 8 );
            memcpy( &secondHash, secondBuffer, 8 );
            size_t ind = (firstHash + i * secondHash) % container.size();
            if (container.isTrue(ind) == false) return false;
            }
        //If all bits were true, the string is likely inserted, but false positive is possible
        return true;
    }

    size_t size( ) const
        {
        return container.size( ) * sizeof( int );
        }

private:
    //Add any private member variables that may be neccessary
    bitmap container;
    size_t numHash;
    std::pair<uint64_t, uint64_t> hash(const String& datum) {
        //Use MD5 to hash the string into one 128 bit hash, and split into 2 hashes
        unsigned char buffer[ MD5_DIGEST_LENGTH ];
        MD5( (const unsigned char *)datum.cstr(), datum.size(), buffer) ;
        //This line is for compiling, replace this with own code
        uint64_t first, second;
        memcpy( &first, buffer, 8 );
        memcpy( &second, buffer + 8, 8 );
        return {first, second};
    }
};

#endif