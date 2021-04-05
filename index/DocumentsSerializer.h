#pragma once
#include "../utility/HashTable.h"
#include "../utility/Vector.h"
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
#include <cstring>

struct DocumentDetails {
    DocumentDetails(const char* u, const char* t, Location lengthOfDocument, Location numUniqueWords) : url(u), title(t), lengthOfDocument(lengthOfDocument), numUniqueWords(numUniqueWords) {}
    String url;
    String title;
    Location lengthOfDocument;
    Location numUniqueWords;
};

struct DocumentBlob {
    char URL[MAX_URL_LENGTH];
    char title[MAX_TITLE_LENGTH];
    Location lengthOfDocument;
    Location numUniqueWords;
    // TODO: Why setting to static fix linker error
    // TODO: Abstract this
    static size_t RoundUp( size_t length, size_t boundary )
       {
       // Round up to the next multiple of the boundary, which
       // must be a power of 2.

       static const size_t oneless = boundary - 1,
          mask = ~( oneless );
       return ( length + oneless ) & mask;
       }
    
    static size_t BytesRequired(const DocumentDetails * doc) {
        return 2 * sizeof(Location) + MAX_URL_LENGTH + MAX_TITLE_LENGTH;
    }
    
    static size_t BytesRequired(const ::vector<DocumentDetails*> * docs) {
        size_t bytes = 0;
        for(size_t i = 0; i < docs->size(); ++i) {
            // TODO: is this jank?
            bytes += BytesRequired(docs->operator[](i));
        }
        return bytes;
    }
    // TODO: Catch errors or handle them somehow
    static char *Write( char *buffer, char *bufferEnd, const DocumentDetails * doc ) {
        if ( !doc ) {
            return buffer;
        }
        size_t bytes = BytesRequired( doc );
        if ( bufferEnd - buffer < bytes )
           throw "Not enough memory allocated!";
        
        DocumentBlob *head = new( ( void * )buffer ) DocumentBlob;
        head->lengthOfDocument = doc->lengthOfDocument;
        head->numUniqueWords = doc->numUniqueWords;
        if(strlen(doc->url.cstr()) > MAX_URL_LENGTH) {
            throw "URL is greater than max url length";
        } else {
            strcpy(head->URL, doc->url.cstr());
        }
        if(doc->title.size() > MAX_TITLE_LENGTH) {
            strncpy(head->title, doc->title.cstr(), MAX_TITLE_LENGTH - 1);
            head->title[MAX_TITLE_LENGTH - 1] = '\0';
        }
        else {
            strcpy(head->title, doc->title.cstr());
        }
        return buffer + RoundUp( bytes, sizeof( size_t ) );
    }
    /*
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
    */
};

class DocumentFile
   {
   private:

      DocumentBlob *blob;

      size_t FileSize( int f )
         {
         struct stat fileInfo;
         fstat( f, &fileInfo );
         return fileInfo.st_size;
         }

   public:

      const DocumentBlob *Blob( )
         {
         return blob;
         }

      DocumentFile( const char *filename )
         {
         // Open the file for reading, map it, check the header,
         // and note the blob address.

         // Your code here.
         int f = open( filename, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO );
         if ( f == -1 )
            std::cerr << "Error openning file " << filename << " with errno = " << strerror( errno ) << std::endl;
         size_t fileSize = FileSize( f );
         blob = ( DocumentBlob * )mmap( nullptr, fileSize,
            PROT_READ, MAP_SHARED, f, 0 );
         if ( blob != MAP_FAILED )
            {
            // TODO: check the blob header ( MagicNumber, Version ) here
            }
         close( f );
         }
       
       //using HashBucket = Bucket< String, TermPostingList* >;

      DocumentFile( const char *filename, const ::vector<DocumentDetails *> * docs )
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
         size_t fileSize = DocumentBlob::BytesRequired( docs );
         ftruncate( f, fileSize );
             char * blobBytes = (char*) blob;

        char * blobEnd = blobBytes + fileSize;
         blob = ( DocumentBlob * )mmap( nullptr, fileSize,
            PROT_READ | PROT_WRITE, MAP_SHARED, f, 0 );
             if ( blob != MAP_FAILED ) {
                 for(size_t i = 0; i < docs->size(); ++i) {
                     blobBytes = DocumentBlob::Write(blobBytes, blobEnd, docs->operator[](i));
                 }
             }
             else {
                std::cerr << "Map failed with errno = " << strerror( errno ) << std::endl;
                exit(0);
                // TODO: ADD not exiting status checks
             }
             close( f );
         }

      ~DocumentFile( )
         {
         // Your code here.
         // HashBlob::Discard( blob );
         }
   };

