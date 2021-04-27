#include <iostream>
#include <fstream>
#include "query_Compiler.h"
#include "constraint_solver.h"
#include "IndexConstructor.h"
#include "Dictionary.h"
#include "mString.h"
#include "Results.h"
#include <filesystem>
#include <string>

#pragma once
#define ASSERT(left,operator,right) { if(!((left) operator (right))){ std::cerr << "ASSERT FAILED: " << #left << #operator << #right << " @ " << __FILE__ << " (" << __LINE__ << "). " << #left << "=" << (left) << "; " << #right << "=" << (right) << std::endl; } }
using namespace std;

int main(int argc, char *argv[]) {
   // ifstream docs;
   // docs.open("testdoc.txt");
   // if (docs.bad()) cerr << "1111";
   // if (docs.fail()) cerr << "Error: " << strerror(errno);
   // if (docs.is_open())
   //    cout<<"Open!!!\n";
   // IndexConstructor ic(0);
   // String term;
   // char term0[10];
   // char* pos=term0;
   // char charr;
   // while (docs.get(charr))
   //    {
   //    if (charr == '#')
   //       break;
   //    else if (charr == '/') 
   //       {
   //       ic.Insert("cat_title", "cat.com");
   //       // cout<<"URL Inserted!\n";
   //       }
   //    else if (charr != '\t')
   //       *pos++ = charr;
   //    else if (pos != term0)
   //       {
   //       *pos = 0;
   //       pos = term0;
   //       String term(term0);
   //       ic.Insert(term, Body);
   //       // cout<<"Inserted "<<term0<<"\n";
   //       // std::cout << "Metadta chunk ends in TEST>CPP: " << std::endl;
   //       ::vector<Location> chunkEnds = ic.fileManager.getChunkEndLocations();
   //       // for( unsigned int i = 0; i < chunkEnds.size(); ++i) {
   //       //    std::cout << chunkEnds[i] ;
   //       // }
   //       // std::cout << std::endl;
   //       }
   //    }
   // ic.FinishConstruction();
   // FileManager manager = ic.fileManager;
   Dictionary dict(0);
   // char word[] = {'c', 'a', 't', '\0'};
   // ISRWord isrword = *dict.OpenISRWord(word);

//    // helper: create three ISRWord items
    ISREndDoc *EndDoc = dict.OpenISREndDoc();


   ::vector<url_score> results1 = Results ( &dict, "google | facebook" );

   std::cerr << "q1 Results:------------------------------" << endl;
  
    printRanks(results1);
   
    cout << "--------------------------------------" << endl;



   // query 2: "quick brown quick"
   cout << "q2 Results:--------------------------------" << endl;
   ::vector<url_score> results2 = Results ( &dict, "select your language" );
   printRanks(results2);
   cout << "--------------------------------------" << endl;



// query 3: quick fox
   cout << "q3 Results:---------------------------------" << endl;
   ::vector<url_score> results3 = Results ( &dict, "select your location" );
   printRanks(results3);
   cout << "--------------------------------------" << endl;

   return 0;
   }
