/*
#pragma once

// HashBlob, a serialization of a HashTable into one contiguous
// block of memory, possibly memory-mapped to a HashFile.

// Nicole Hamilton  nham@umich.edu

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <cstdint>
//#include <malloc.h>
#include <unistd.h>
#include <sys/mman.h>
#include <iostream>

#include "HashTable.h"


using Hash = HashTable< const char *, size_t >;
using Pair = Tuple< const char *, size_t >;
using HashBucket = Bucket< const char *, size_t >;


static const size_t Unknown = 0;


size_t RoundUp( size_t length, size_t boundary )
   {
   // Round up to the next multiple of the boundary, which
   // must be a power of 2.

   static const size_t oneless = boundary - 1,
      mask = ~( oneless );
   return ( length + oneless ) & mask;
   }


struct SerialTuple
   {
   // This is a serialization of a HashTable< char *, size_t >::Bucket.
   // One is packed up against the next in a HashBlob.

   // Since this struct includes size_t and uint32_t members, we'll
   // require that it be sizeof( size_t ) aligned to avoid unaligned
   // accesses.

   public:

      // SerialTupleLength = 0 is a sentinel indicating
      // this is the last SerialTuple chained in this list.
      // (Actual length is not given but not needed.)

      size_t Length, Value;
      uint32_t HashValue;

      // The Key will be a C-string of whatever length.
      char Key[ Unknown ];

      // Calculate the bytes required to encode a HashBucket as a
      // SerialTuple.

      static size_t BytesRequired( const HashBucket *b )
         {
         // Your code here.
         size_t a, bt, c, d;
         a = 2 * sizeof( size_t );
         bt = sizeof( uint32_t );
         c = sizeof( Key );
         d = strlen( b->tuple.key );
         size_t res = 2 * sizeof( size_t ) + sizeof( uint32_t ) + sizeof( Key ) + strlen( b->tuple.key ) + 1;
         return res;
         }

      // Write the HashBucket out as a SerialTuple in the buffer,
      // returning a pointer to one past the last character written.

      static char *Write( char *buffer, char *bufferEnd,
            const HashBucket *b )
         {
         // Your code here.
         if ( !b )
            {
            SerialTuple *sentinel = new( ( void * )buffer ) SerialTuple;
            sentinel->Length = 0;
            return buffer + RoundUp( sizeof( SerialTuple ), sizeof( size_t ) );
            }
         size_t bytes = BytesRequired( b );
         if ( bufferEnd - buffer < bytes )
            throw "Not enough memory allocated!";
         SerialTuple *head = new( ( void * )buffer ) SerialTuple;
         head->Length = bytes;
         head->Value = b->tuple.value;
         head->HashValue = b->hashValue;
         memcpy( head->Key, b->tuple.key, strlen( b->tuple.key ) + 1 );
         return buffer + RoundUp( bytes, sizeof( size_t ) );
         }
  };


class HashBlob
   {
   // This will be a hash specifically designed to hold an
   // entire hash table as a single contiguous blob of bytes.
   // Pointers are disallowed so that the blob can be
   // relocated to anywhere in memory

   // The basic structure should consist of some header
   // information including the number of buckets and other
   // details followed by a concatenated list of all the
   // individual lists of tuples within each bucket.

   public:

      // Define a MagicNumber and Version so you can validate
      // a HashBlob really is one of your HashBlobs.

      size_t MagicNumber,
         Version,
         BlobSize,
         NumberOfBuckets,
         Buckets[ Unknown ];

      // The SerialTuples will follow immediately after.


      const SerialTuple *Find( const char *key ) const
         {
         // Search for the key k and return a pointer to the
         // ( key, value ) entry.  If the key is not found,
         // return nullptr.

         // Your code here.
         uint32_t hashValue = fnvHash( key, strlen( key ) );
         size_t bucket = Buckets[ hashValue % NumberOfBuckets ];
         if ( !bucket )
            return nullptr;
         // get the location of the list
         const char *iter = ( const char * )this + bucket;
         SerialTuple *serialIter = ( SerialTuple * )iter;
         while ( serialIter->Length )
            {
            if ( CompareEqual( key, serialIter->Key ) )
               return serialIter;
            iter += RoundUp( serialIter->Length, sizeof( size_t ) );
            serialIter = ( SerialTuple * )iter;
            }

         return nullptr;
         }


      static size_t BytesRequired( const Hash *hashTable )
         {
         // Calculate how much space it will take to
         // represent a HashTable as a HashBlob.

         // Need space for the header + buckets +
         // all the serialized tuples.

         // Your code here.
         size_t headerSize = sizeof( HashBlob );

         size_t bucketsSize = sizeof( size_t ) * hashTable->numberOfBuckets;
         size_t tupleSize = 0, 
               sentinalSize = RoundUp( sizeof( SerialTuple ), sizeof( size_t ) );
         for ( size_t i = 0; i < hashTable->numberOfBuckets; ++i )
            if ( hashTable->buckets[ i ] )
               {
               HashBucket *curr = hashTable->buckets[ i ];
               while ( curr )
                  {
                  tupleSize += RoundUp( SerialTuple::BytesRequired( curr ), sizeof( size_t ) );
                  curr = curr->next;
                  }
               tupleSize += sentinalSize;
               }

         return headerSize + bucketsSize + tupleSize;
         }

      // Write a HashBlob into a buffer, returning a
      // pointer to the blob.

      static HashBlob *Write( HashBlob *hb, size_t bytes,
            const Hash *hashTable )
         {
         // Your code here.
         // caller guarantees the memory is enough

         char *blobByte = ( char * )hb;
         hb->BlobSize = bytes;
         hb->MagicNumber = 0;
         hb->Version = 0;
         hb->NumberOfBuckets = hashTable->numberOfBuckets;
         // fill the buckets as well as the lists
         size_t listEnd = sizeof( HashBlob ) + hashTable->numberOfBuckets * sizeof( size_t );  // store where the last list ends
         char *serialLoc = blobByte + listEnd;
         for ( size_t i = 0; i < hashTable->numberOfBuckets; ++i )
            {
            if ( hashTable->buckets[ i ] )
               {
               *( hb->Buckets + i ) = listEnd;
               HashBucket *curr = hashTable->buckets[ i ];
               while ( curr )
                  {
                  // written bytes number should be RoundupBy8( 20 + strLen )
                  serialLoc = SerialTuple::Write( serialLoc, blobByte + bytes, curr );
                  curr = curr->next;
                  }
               serialLoc = SerialTuple::Write( serialLoc, blobByte + bytes, nullptr );  // write 24 bytes
               listEnd = serialLoc - blobByte;
               }
            else
               *( hb->Buckets + i ) = 0;
            assert( serialLoc - blobByte <= hb->BlobSize );
            }

         return hb;
         }

      // Create allocates memory for a HashBlob of required size
      // and then converts the HashTable into a HashBlob.
      // Caller is responsible for discarding when done.

      // (No easy way to override the new operator to create a
      // variable sized object.)

      static HashBlob *Create( const Hash *hashTable )
         {
         // Your code here.
         size_t bytes = BytesRequired( hashTable );
         char *blobByte = new char[ bytes ];
         HashBlob *blob = ( HashBlob * )blobByte;
         return Write( blob, bytes, hashTable );
         }

      // Discard

      static void Discard( HashBlob *blob )
         {
         // Your code here.
         char *blobByte = ( char * )blob;
         delete [] blobByte;
         }
   };

class HashFile
   {
   private:

      HashBlob *blob;

      size_t FileSize( int f )
         {
         struct stat fileInfo;
         fstat( f, &fileInfo );
         return fileInfo.st_size;
         }

   public:

      const HashBlob *Blob( )
         {
         return blob;
         }

      HashFile( const char *filename )
         {
         // Open the file for reading, map it, check the header,
         // and note the blob address.

         // Your code here.
         int f = open( filename, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO );
         if ( f == -1 )
            std::cerr << "Error openning file " << filename << " with errno = " << strerror( errno ) << std::endl;
         size_t fileSize = FileSize( f );
         blob = ( HashBlob * )mmap( nullptr, fileSize, 
            PROT_READ, MAP_SHARED, f, 0 );
         if ( blob != MAP_FAILED )
            {
            // TODO: check the blob header ( MagicNumber, Version ) here
            }
         close( f );
         }

      HashFile( const char *filename, const Hash *hashtable )
         {
         // Open the file for write, map it, write
         // the hashtable out as a HashBlob, and note
         // the blob address.

         // Your code here.
         int f = open( filename, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO );  // if file not exist, create it
         if ( f == -1)
            {
            std::cout << "Failed to open file: " << filename << ", with error number " << errno << std::endl;
            }
         size_t fileSize = HashBlob::BytesRequired( hashtable );
         ftruncate( f, fileSize );
         blob = ( HashBlob * )mmap( nullptr, fileSize,
            PROT_READ | PROT_WRITE, MAP_SHARED, f, 0 );
         if ( blob != MAP_FAILED )
            blob = HashBlob::Write( blob, fileSize, hashtable );
         else
            std::cerr << "Map failed with errno = " << strerror( errno ) << std::endl;
         close( f );
         }

      ~HashFile( )
         {
         // Your code here.
         // HashBlob::Discard( blob );
         }
   };
*/