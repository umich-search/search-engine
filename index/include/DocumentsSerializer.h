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
    
    // get bytes required for document
    static size_t BytesRequired(const SharedPointer<DocumentDetails> doc){
        return 2 * sizeof(Location) + MAX_URL_LENGTH + MAX_TITLE_LENGTH;
    }
    
    // get bytes required for list of documents
    static size_t BytesRequired(const SharedPointer<::vector<SharedPointer<DocumentDetails>>> docs) {
        size_t bytes = 0;
        for(size_t i = 0; i < docs->size(); ++i) {
            bytes += BytesRequired(docs->operator[](i));
        }
        return bytes;
    }

    // write document to buffer
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
