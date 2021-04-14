#include "../include/Dictionary.h"
#include "../include/IndexConstructor.h"
#include <iostream>
#include <filesystem>



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
         */
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

int main() {
    std::__fs::filesystem::remove_all(CHUNK_DIRECTORY);
    std::__fs::filesystem::create_directory(CHUNK_DIRECTORY);

    IndexConstructor ic;
    for(unsigned int j = 0; j < 100; ++j ) {
        for(unsigned int i = 0; i < 100; ++i) {
            ic.Insert("Hello", Body);
            ic.Insert("How are", Body);
            ic.Insert("You",Body);
        }
        String t("Title");
        String u("URL");
        ic.Insert(t,u);
    }
    validateChunksIntegrity();
    std::__fs::filesystem::remove_all(CHUNK_DIRECTORY);
}
