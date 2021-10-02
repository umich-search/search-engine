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

struct SerialEndDocs {
public:
    size_t Length;
    char DynamicSpace[ Unknown ];

    // Bytes required for EndDoc postings
    static size_t BytesRequired ( const CommonHeader * header) {
        size_t typeSize, numOfOccurenceSize, numOfDocumentSize, termSize;
        typeSize = sizeof(int);
        numOfDocumentSize = sizeof(w_Occurence);
        numOfOccurenceSize = sizeof(d_Occurence);
        termSize = header->term.size() + 1;
        return typeSize + numOfOccurenceSize + numOfDocumentSize + termSize;
    }
    
    // Bytes required to encode synchronization index
    static size_t BytesRequired( const SyncIndex *syncIndex) {
       return syncIndex->postingsListOffset.size() * 2 * sizeof(size_t);
    }
    // Bytes required to encode EndDocPostingList
    static size_t BytesRequired( const SharedPointer<EndDocPostingList> p) {
    /*
       TERM POSTING LIST DATA TO SERIALIZE:
       CommonHeader header
       SyncIndex syncIndex
       vector<IPostTerm> posts
    */
      size_t headerSize, syncIndexSize, endDocsSize, memberVarSize;
      headerSize = BytesRequired(&p->header);
      syncIndexSize = BytesRequired(&p->syncIndex);
        endDocsSize = 0;
      for(unsigned int i = 0; i < p->posts.size(); ++i) {
          endDocsSize += UtfBytes(p->posts[i].delta);
      }
        memberVarSize = sizeof(Offset);
      return headerSize + syncIndexSize + endDocsSize + memberVarSize;
    }
    
    // Write to buffer
    static char *Write( char *buffer, char *bufferEnd, const SharedPointer<EndDocPostingList> p ) {
       if ( !p ) {
           return buffer + RoundUp( sizeof( SerialEndDocs ), sizeof(size_t));
       }
        size_t bytes = BytesRequired( p );
       if ( bufferEnd - buffer < bytes )
          throw "Not enough memory allocated!";

       SerialEndDocs *head = new( ( void * )buffer ) SerialEndDocs;
        head->Length = bytes;
       char * curr = head->DynamicSpace;
        
       // Copy header
       memcpy(curr, &p->header.numOfOccurence, sizeof(w_Occurence));
       memcpy(curr + sizeof(w_Occurence), &p->header.numOfDocument, sizeof(d_Occurence));
       memcpy(curr + sizeof(w_Occurence) + sizeof(d_Occurence), &p->header.type, sizeof(int));
       memcpy(curr + sizeof(int) + sizeof(w_Occurence) + sizeof(d_Occurence), p->header.term.cstr(), p->header.term.size() + 1);
       curr += BytesRequired(&p->header);
       for(size_t i = 0; i < p->syncIndex.postingsListOffset.size(); ++i) {
           memcpy(curr, &p->syncIndex.postingsListOffset[i], sizeof(size_t));
           curr += sizeof(size_t);
       }
       for(size_t i = 0; i < p->syncIndex.postLocation.size(); ++i) {
           memcpy(curr, &p->syncIndex.postLocation[i], sizeof(size_t));
           curr += sizeof(size_t);
       }
        for(size_t i = 0; i < p->posts.size(); ++i) {
            size_t utfBytes = IntToUtf(p->posts[i].delta, (uint8_t*)curr);
            curr += utfBytes;
        }
       return buffer + RoundUp( bytes, sizeof( size_t ) );
       }
    
    // Create allocates memory
    // Caller is responsible for discarding when done.
    static SerialEndDocs *Create( const SharedPointer<EndDocPostingList> endDocs )
       {
       size_t bytes = BytesRequired( endDocs );
       char *blobByte = new char[ bytes ];
           Write( blobByte, blobByte + bytes, endDocs );
           return ( SerialEndDocs *) blobByte;
       }

    // Discard
    static void Discard( SerialEndDocs *blob ) {
        char *blobByte = ( char * )blob;
        delete [] blobByte;
    }

};
