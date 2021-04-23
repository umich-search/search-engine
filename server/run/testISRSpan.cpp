#include <iostream>
#include <fstream>
#include "../../query_compiler/include/query_Compiler.h"
#include "../../constraint_solver/include/constraint_solver.h"
#include "../../index/include/IndexConstructor.h"
#include "../../index/include/Dictionary.h"
#include "../../utility/include/mString.h"
#include "Results.h"
#include <filesystem>
#include <string>

#pragma once
#define ASSERT(left, operator, right) { if(!((left) operator (right))){ std::cerr << "ASSERT FAILED: " << #left << #operator << #right << " @ " << __FILE__ << " (" << __LINE__ << "). " << #left << "=" << (left) << "; " << #right << "=" << (right) << std::endl; } }
using namespace std;

int main(int argc, char *argv[]) {
    std::__fs::filesystem::remove_all(CHUNK_DIRECTORY);
    std::__fs::filesystem::create_directory(CHUNK_DIRECTORY);

    ifstream docs;
    docs.open("testdoc.txt");
    IndexConstructor ic(0);
    String term;
    char term0[10];
    char *pos = term0;
    char charr;
    while (docs.get(charr)) {
        if (charr == '#')
            break;
        else if (charr == '/') {
            ic.Insert("cat_title", "cat.com");
            // cout<<"URL Inserted!\n";
        } else if (charr != '\t')
            *pos++ = charr;
        else if (pos != term0) {
            *pos = 0;
            pos = term0;
            String term(term0);
            ic.Insert(term, Body);
            // cout<<"Inserted "<<term0<<"\n";
            // std::cout << "Metadta chunk ends in TEST>CPP: " << std::endl;
            ::vector<Location> chunkEnds = ic.fileManager.getChunkEndLocations();
            // for( unsigned int i = 0; i < chunkEnds.size(); ++i) {
            //    std::cout << chunkEnds[i] ;
            // }
            // std::cout << std::endl;
        }
    }
    ic.FinishConstruction();
    FileManager manager = ic.fileManager;
    Dictionary dict(0);
    // char word[] = {'c', 'a', 't', '\0'};
    // ISRWord isrword = *dict.OpenISRWord(word);

//    // helper: create three ISRWord items
    ISREndDoc *EndDoc = dict.OpenISREndDoc();

    char quick[] = {'q', 'u', 'i', 'c', 'k', '\0'};
    char brown[] = {'b', 'r', 'o', 'w', 'n', '\0'};
    char fox[] = {'f', 'o', 'x', '\0'};

    ISRWord *word_quick = dict.OpenISRWord(quick);
    ISRWord *word_brown = dict.OpenISRWord(brown);
    ISRWord *word_fox = dict.OpenISRWord(fox);
    ISRWord *terms[] = {word_quick, word_brown, word_fox};
    char *input = "quick | brown | fox";
    ISR *queryRoot = Query_Compiler(&dict, input);
    ::vector<Match *> * match = (::vector<Match *> *) ConstraintSolver(EndDoc, queryRoot);
    cout << "start location"<<(*match)[1]->start << endl;
    struct Weights weights {
            weightShortSpan: 1,
            weightOrderSpan: 1,
            weightPhrase: 1,
            weightTopSpan: 0.5,
            weightAll: 1,
            weightMost: 1,
            weightSome: 5
    };
    float score = calculate_scores((*match)[1], (ISRWord **)terms, 3, 2, &weights);
    cout<<score;
    return 0;
}
