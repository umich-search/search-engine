#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <cstdint>
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>

struct DocumentDetails {
    DocumentDetails(const char* u, const char* t, Location lengthOfDocument, Location numUniqueWords) :
                        url(u),
                        title(t),
                        lengthOfDocument(lengthOfDocument),
                        numUniqueWords(numUniqueWords)
                        {}
    String title;
    String url;
    Location lengthOfDocument;
    Location numUniqueWords;

};

struct DocumentBlob {
    Location lengthOfDocument;
    Location numUniqueWords;
    char URL[MAX_URL_LENGTH];
    char title[MAX_TITLE_LENGTH];
    
    static size_t BytesRequired(const SharedPointer<DocumentDetails> doc){
        return 2 * sizeof(Location) + MAX_URL_LENGTH + MAX_TITLE_LENGTH;
    }
    
    static size_t BytesRequired(const SharedPointer<::vector<SharedPointer<DocumentDetails>>> docs) {
        size_t bytes = 0;
        for(size_t i = 0; i < docs->size(); ++i) {
            bytes += BytesRequired(docs->operator[](i));
        }
        return bytes;
    }

    static char *Write( char *buffer, char *bufferEnd, const SharedPointer<DocumentDetails> doc){
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
};
/*
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

         int f = open( filename, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO );
         if ( f == -1 )
            std::cerr << "Error openning file " << filename << " with errno = " << strerror( errno ) << std::endl;
         size_t fileSize = FileSize( f );
         blob = ( DocumentBlob * )mmap( nullptr, fileSize,
            PROT_READ, MAP_SHARED, f, 0 );
         if ( blob != MAP_FAILED )
            {
            }
         close( f );
         }
       
      DocumentFile( const char *filename, const ::vector<DocumentDetails *> * docs )
         {
         int f = open( filename, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO );  // if file not exist, create it
         if ( f == -1)
            {
            // std::cout << "Failed to open file: " << filename << ", with error number " << errno << std::endl;
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
             }
             close( f );
         }

      ~DocumentFile( )
         {
         // HashBlob::Discard( blob );
         }
   };
*/
