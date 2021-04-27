#include "../include/IndexConstructor.h"
#include <iostream>
#include <filesystem>



int main() {
  //  std::__fs::filesystem::remove_all(CHUNK_DIRECTORY);
    //std::__fs::filesystem::create_directory(CHUNK_DIRECTORY);
    size_t total_num_docs = 0;
    size_t total_num_words = 0;
    size_t num_words_in_chunk = 0;
    ::vector<size_t> num_docs_in_thread;
    ::vector<size_t> num_words_in_thread;
    
    for(unsigned int i = 0; i < 1; ++i) {
        FileManager m(i);
        EndDocPostingListRaw endDocList = m.GetEndDocList();
        size_t currDocIndex = 0;
        size_t numChunks = m.getNumChunks();
        std::cout << "Num chunks: " << m.getNumChunks() << std::endl;
        for(unsigned int currChunk = 0; currChunk < m.getNumChunks() - 1; ++currChunk) {
            std::cout << "NEW CHUNK" << std::endl;
            EndDocPostingListRaw endDocList = m.GetEndDocList(currChunk);
            std::cout <<"Num documents: " << endDocList.getHeader()->numOfDocument;
            for(unsigned int docIndex = 0; docIndex < endDocList.getHeader()->numOfDocument - 1; ++docIndex) {
                if(docIndex == 80) {
                    std::cout << "" << std::endl;
                }
                DocumentDetails d = m.GetDocumentDetails(currDocIndex, currChunk);
                currDocIndex++;
                std::cout << "Document " << docIndex << " " << currDocIndex << ": " << "Title: " << d.title.cstr() << " URL: " << d.url.cstr() << " Unique Words: " << d.numUniqueWords << " Length: " << d.lengthOfDocument << std::endl;
            }
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
