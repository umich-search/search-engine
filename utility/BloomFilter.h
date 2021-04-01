#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include "bitmap.h"
#include <cmath>
#include <string.h>
#include <string>
// #include "String.h"
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

    void insert(const string& s) {
        //Hash the string into two unique hashes
        std::pair<uint64_t, uint64_t> myHash = hash(s);
        uint64_t first = myHash.first;
        uint64_t second = myHash.second;
        string firstStr = to_string(first);
        string secondStr = to_string(second);
        //Use double hashing to get unique bit, and repeat for each hash function
        for ( int i = 0; i < numHash; ++i )
            {
            uint64_t firstHash, secondHash;
            unsigned char firstBuffer[ MD5_DIGEST_LENGTH ], secondBuffer[ MD5_DIGEST_LENGTH ];
            MD5( (const unsigned char *)firstStr.c_str(), firstStr.size(), firstBuffer); //TODO: change datum.size
            MD5( (const unsigned char *)secondStr.c_str(), secondStr.size(), secondBuffer);
            memcpy( &firstHash, firstBuffer, 8 );
            memcpy( &secondHash, secondBuffer, 8 );
            size_t ind = (firstHash + i * secondHash) % container.size();
            container.set(ind);
            }
    }

    bool contains(const std::string& s) {
        //Hash the string into two unqiue hashes
        std::pair<uint64_t, uint64_t> myHash = hash(s);
        uint64_t first = myHash.first;
        uint64_t second = myHash.second;
        string firstStr = to_string(first);
        string secondStr = to_string(second);
        //Use double hashing to get unique bit, and repeat for each hash function
        //If bit is false, we know for certain this unique string has not been inserted
        for ( int i = 0; i < numHash; ++i )
            {
            uint64_t firstHash, secondHash;
            unsigned char firstBuffer[ MD5_DIGEST_LENGTH ], secondBuffer[ MD5_DIGEST_LENGTH ];
            MD5( (const unsigned char *)firstStr.c_str(), firstStr.size(), firstBuffer); //TODO: change datum.size
            MD5( (const unsigned char *)secondStr.c_str(), secondStr.size(), secondBuffer);
            memcpy( &firstHash, firstBuffer, 8 );
            memcpy( &secondHash, secondBuffer, 8 );
            size_t ind = (firstHash + i * secondHash) % container.size();
            if (container.isTrue(ind) == false) return false;
            }
        //If all bits were true, the string is likely inserted, but false positive is possible
        return true;
    }

private:
    //Add any private member variables that may be neccessary
    bitmap container;
    size_t numHash;
    std::pair<uint64_t, uint64_t> hash(const std::string& datum) {
        //Use MD5 to hash the string into one 128 bit hash, and split into 2 hashes
        unsigned char buffer[ MD5_DIGEST_LENGTH ];
        MD5( (const unsigned char *)datum.c_str(), datum.size(), buffer) ;
        //This line is for compiling, replace this with own code
        uint64_t first, second;
        memcpy( &first, buffer, 8 );
        memcpy( &second, buffer + 8, 8 );
        return {first, second};
    }
};

#endif