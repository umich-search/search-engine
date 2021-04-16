#include "../include/Dictionary.h"
#include "../include/IndexConstructor.h"
#include <iostream>
#include <filesystem>


/*
int validateChunksIntegrity() {
    FileManager m;
    vector<Location> endLocs = m.getChunkEndLocations(); 
    vector<d_Occurence> docCounts = m.getDocCountsAfterChunk();


    std::cout << "Index Statistics: " << std::endl;
    std::cout << "Number of chunks:  " << m.getNumChunks();
    std::cout << std::endl;

    std::cout << "Chunk end locations: ";
    for(unsigned int i = 0; i < endLocs.size(); ++i) {
        std::cout << endLocs[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "Number of documents after each chunk : ";
    for(unsigned int i = 0; i < docCounts.size(); ++i) {
        std::cout << docCounts[i] << " ";
    }
    std::cout << std::endl;
    size_t wordCount = 0;
    size_t docLoc = 0;
    for(unsigned int i = 0; i < m.getNumChunks(); ++i) {
        EndDocPostingListRaw endDocList = m.GetEndDocList(i);
        std::cout << "Looking at end doc list for chunk num: " << i << std::endl;
        for(unsigned int j = 0; j < endDocList.getHeader()->numOfDocument; ++j) {
            if(j == 0) {
                docLoc = endLocs[i];
            }
            docLoc += endDocList.getPostAt(j).delta;
            std::cout << docLoc << " ";
        }
        std::cout << std::endl;
        /*
        if(m.ReadChunk(i) == 0) {
            m.termIndexBlob->Find(
        }

              const SerialTuple *Find( const char *key ) const
         {
         // Search for the key k and return a pointer to the
         // ( key, value ) entry.  If the key is not found,
         // return nullptr.

         fnvHash_t hashValue = fnvHash( key, strlen ( key ) );
         size_t bucket = Buckets[ hashValue % NumberOfBuckets ];
         if ( !bucket )
            return nullptr;
         // get the location of the list
         const char *iter = ( const char * )this + bucket;
         SerialTuple *serialIter = ( SerialTuple * )iter;
         while ( serialIter->Length )
            {
            if ( CompareEqual( key, serialIter->DynamicSpace ) )
               return serialIter;
            iter += RoundUp( serialIter->Length, sizeof( size_t ) );
            serialIter = ( SerialTuple * )iter;
            }
         
         return nullptr;
         }
         
    }
            std::cout << "Analyzing documets in chunks: " << std::endl;
        size_t docIndex = 0;
        for(unsigned int i = 0; i < docCounts.size(); ++i) {
            std::cout << "Looking at chunk " << i << std::endl;
                size_t chunkDocs;
                if(i == 0) {
                    chunkDocs = docCounts[i];
                }
                else {
                    chunkDocs = docCounts[i] - docCounts[i - 1];
                }
                for(unsigned int k = 0; k < chunkDocs; ++k) {
                    try {
                         DocumentDetails d = m.GetDocumentDetails(docIndex, i);                        

                        std::cout << "Document in chunk with data: " << std::endl;
                        std::cout << "Title: " << d.title.cstr() << std::endl;
                        std::cout << "URL: " << d.url.cstr() << std::endl;
                        std::cout << "Length: " << d.lengthOfDocument << std::endl;
                        std::cout << "Unique words: " << d.numUniqueWords << std::endl;
 
                    } catch(const char * excp) {   
                        std::cout << "ERROR" << std::endl;
                    }
                    docIndex++;
                }
        }
    return 0;
}
*/
int main() {
  //  std::__fs::filesystem::remove_all(CHUNK_DIRECTORY);
    //std::__fs::filesystem::create_directory(CHUNK_DIRECTORY);
    size_t total_num_docs = 0;
    size_t total_num_words = 0;
    size_t num_words_in_chunk = 0;
    ::vector<size_t> num_docs_in_thread;
    ::vector<size_t> num_words_in_thread;
    
    for(unsigned int i = 0; i < 10; ++i) {
        FileManager m(i);
        if(i == 7) {
            std::cout << "" << std::endl;
        }
        size_t num_chunks = m.getNumChunks();
        ::vector<d_Occurence> docCounts = m.getDocCountsAfterChunk();
        ::vector<Location> endDocLocs = m.getChunkEndLocations();
        std::cout << "Doc counts for thread: " << i << std::endl;
        for(unsigned int i = 0; i < docCounts.size(); ++i) {
            std::cout << docCounts[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "Chunk end locs for thread: " << i << std::endl;
        for(unsigned int i = 0; i < endDocLocs.size(); ++i) {
            std::cout << endDocLocs[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "Num unique words for thread: " << i << std::endl;
        std::cout << m.chunksMetadata->numUniqueWords << std::endl;
        total_num_docs += docCounts[num_chunks - 1];
        total_num_words += endDocLocs[num_chunks - 1];
    }
    std::cout << "total number of documents: " << total_num_docs << std::endl;
    std::cout << "total number of words: " << total_num_words << std::endl;
}
