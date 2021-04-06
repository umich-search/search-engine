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
   static size_t FileSize( int f) {
      struct stat fileInfo;
      fstat( f, &fileInfo );
      return fileInfo.st_size;
   }
   struct ChunksMetadata {
      Offset numChunks;
      w_Occurence numWords;
      w_Occurence numUniqueWords;
      d_Occurence numDocs;
      Location endLocation;
       Offset numDocDetails;
      char docDetailsLoc[];
   };
    // Reads metadata file
    int ReadMetadata();


public:
   HashBlob * termIndexBlob;
   SerialEndDocs * endDocListBlob;
   ChunksMetadata * chunksMetadata;

   void * docsBlob;
   void * chunkDetails;
    
    FileManager() {
        ReadMetadata();
    }

    static int WriteChunk(SharedPointer<HashTable<String, TermPostingList *>> termIndex, 
                  SharedPointer<EndDocPostingList> endDocList, 
                  w_Occurence numWords, 
                  w_Occurence numUniqueWords, 
                  d_Occurence numDocs, 
                  Location endLocation,
                  ::vector<SharedPointer<DocumentDetails>> docDetails,
                 size_t numChunks,
                  const char* filename,
                   const char* docs_filename);

    // Bring chunk into memory
    int ReadChunk(const char *path);
    // Bring documents into memory
    int ReadDocuments(const char *path);
    // Returns term list given term and optional chunk_path
    TermPostingListRaw GetTermList(String &term, const char* chunk_path = nullptr);
    // Returns end doc list given term and optional chunk_path
    EndDocPostingListRaw GetEndDocList(const char* chunk_path = nullptr);
    // Get the number of chunks in the index
    Offset getNumChunks();
    // Get total number of words in index
    w_Occurence getIndexWords();
    // Get total number of unique words in index
    w_Occurence getIndexUniqueWords();
    // get end location of index
    Location getIndexEndLocation();
    // get number of documents in index
    d_Occurence getNumDocuments;
    // get vector of chunk endlocations
    // TODO: Got to change so num words is reset for each chunk
    ::vector<Location> getChunkEndLocations();
    
   
};
