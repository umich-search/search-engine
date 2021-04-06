#pragma once

#include "Global.h"
#include "PostingList.h"
#include "PostingListBlob.h"
#include "DocumentsSerializer.h"
#include "DictionarySerializer.h"
#include "EndDocSerializer.h"
#include <cstring>


class FileManager {
private:
   size_t FileSize( int f) {
      struct stat fileInfo;
      fstat( f, &fileInfo );
      return fileInfo.st_size;
   }
   struct ChunksMetadata {
      Offset numChunks;
      Offset numWords;
      Offset numUniqueWords;
      Offset numDocs;
      Offset endLocation;
       Offset numDocDetails;
      char docDetailsLoc[];
   };

public:
   void * blob;
   HashBlob * termIndexBlob;
   SerialEndDocs * endDocListBlob;
   ChunksMetadata * metadata;

   void * docDetailsBlob;
   void * chunkDetails;

    int WriteChunk(HashTable<String, TermPostingList *> &termIndex, 
                  EndDocPostingList * endDocList, 
                  w_Occurence numWords, 
                  w_Occurence numUniqueWords, 
                  d_Occurence numDocs, 
                  Location endLocation,
                  ::vector<SharedPointer<DocumentDetails>> docDetails,
                 size_t numChunks,
                  const char* filename,
                  const char* docs_filename) 
   {
      int f_chunk = open( filename, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO );  // if file not exist, create it
      if ( f_chunk == -1) {
         std::cout << "Failed to open file: " << filename << ", with error number " << errno << std::endl;
         throw "file open failed";
      }
      // WRITE ENDDOC AND TERMPOSTING LIST TO FILE
      size_t termListSize, endDocEnd, numWordsSize, numUniqueWordsSize, numDocsSize, endLocationSize;
      termListSize = RoundUp(HashBlob::BytesRequired(&termIndex), sizeof(size_t));
       endDocEnd = RoundUp(SerialEndDocs::BytesRequired(endDocList), sizeof(size_t)); // TODO: more dynamic, bytes reuired doesn't include lebgth size
      numWordsSize = sizeof(w_Occurence);
      numUniqueWordsSize = sizeof(w_Occurence);
      numDocsSize = sizeof(d_Occurence);
      endLocationSize = sizeof(endLocation);

       size_t fileSize = endDocEnd + termListSize;// + numWordsSize + numUniqueWordsSize + numDocsSize + endLocationSize;
      ftruncate( f_chunk, fileSize );

      blob = ( HashBlob * )mmap( nullptr, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, f_chunk, 0 );   
      if (blob == MAP_FAILED ) {
         throw "mapping failed";
      }

      else {
          //blob += sizeof(Offset);
          char * start = (char*) blob;
         blob = SerialEndDocs::Write((char*)blob, (char *)blob + endDocEnd, endDocList);
          char * after = (char *) blob;
          size_t diff = after - start;
          //char *curr = (char*)blob + endDocEnd;
         blob = HashBlob::Write((HashBlob *)blob, termListSize, &termIndex);
      }
      close(f_chunk);
      // READ FIRST

      int f_chunk_meta = open( CHUNKS_META_FILENAME, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO );  // if file not exist, create it
      if ( f_chunk == -1) {
         std::cout << "Failed to open file: " << CHUNKS_META_FILENAME << ", with error number " << errno << std::endl;
         throw "file open failed";
      }

      // WRITE DOC DETAILS TO FILE
      // TODO: Check map failed
      size_t docDetailsSize = docDetails.size() * DOCUMENT_SIZE;
      int f_doc_details = open( docs_filename, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO );
      if (f_doc_details == -1) {
         std::cout << "Failed to open file: " << docs_filename << ", with error number " << errno << std::endl;
          throw "file open failed";
      }
      ftruncate(f_doc_details, docDetailsSize);
      docDetailsBlob = mmap( nullptr, docDetailsSize, PROT_READ | PROT_WRITE, MAP_SHARED, f_doc_details, 0);
       for(size_t i = 0; i < docDetails.size(); ++i){
         docDetailsBlob = DocumentBlob::Write((char *)docDetailsBlob, (char *)docDetailsBlob + DOCUMENT_SIZE, docDetails[i]);
      }
      close(f_doc_details);
       
       // UPDATE METADATA
       int f_metadata = open( CHUNKS_META_FILENAME, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO );
       if (f_metadata == -1) {
           std::cout << "Failed to open file: " << docs_filename << ", with error number " << errno << std::endl;
            throw "file open failed";
       }
       size_t og = FileSize(f_metadata);
       if(FileSize(f_metadata) == 0) {
           ftruncate(f_metadata, sizeof(ChunksMetadata) + sizeof(Location));
           og = FileSize(f_metadata);

            metadata = (ChunksMetadata*)mmap( nullptr, FileSize(f_metadata), PROT_READ | PROT_WRITE, MAP_SHARED, f_metadata, 0);
           metadata->numWords = 0;
           metadata->numDocs = 0;
           metadata->endLocation = 0;
           metadata->numUniqueWords = 0;
           metadata->numChunks = 0;
           metadata->numDocDetails = 0;
           char * curr = ((char*)metadata + sizeof(ChunksMetadata));
           memcpy(curr, (char*)&endLocation, sizeof(Location));
       }
       else {
           ftruncate(f_metadata, FileSize(f_metadata) + sizeof(Location));// * (numChunks + 1));
           
           metadata = (ChunksMetadata*)mmap( nullptr, FileSize(f_metadata), PROT_READ | PROT_WRITE, MAP_SHARED, f_metadata, 0);
           metadata->numWords = numWords;
           metadata->numDocs = numDocs;
           metadata->endLocation = endLocation;
           metadata->numUniqueWords = numUniqueWords;
           metadata->numChunks++;
           char * curr = (char*)metadata + sizeof(ChunksMetadata) + sizeof(Location) * metadata->numDocDetails;
           memcpy(curr, (char*)&endLocation, sizeof(Location));
           metadata->numDocDetails++;

       }

       close(f_metadata);

       
       
       
       return 0;
   }

    int ReadChunk(const char *filename) {
        
         int chunkFile = open( filename, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO );
        if (chunkFile == -1 ) {
            std::cerr << "Error openning file " << filename << " with errno = " << strerror( errno ) << std::endl;
         }
        
        size_t fileSize = FileSize(chunkFile);
        blob = ( HashBlob *)mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, chunkFile, 0);
        //size_t endDocListSize = *(size_t*)blob;
        endDocListBlob = (SerialEndDocs *)blob;
        size_t endDocEnd = RoundUp(endDocListBlob->Length, sizeof(size_t)); // TODO: more dynamic, bytes reuired doesn't include lebgth size

        char* curr = (char*)blob + endDocEnd; //+ sizeof(Offset);
        termIndexBlob = (HashBlob *)curr;
        close(chunkFile);
        return 0;

    }
   
};
