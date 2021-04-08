#include <cassert>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <vector>

#include "HashTable.h"
#include "./Common.h"
#include "Timer.h"

using namespace std;


void Usage() {
   cout <<

        "Usage:  HashTable [ -vL ] wordsin.txt\n"
        "\n"
        "Reads words from wordsin.txt, creates a HashTable\n"
        "in memory, optimizes it, then searches it for words\n"
        "read from cin.\n"
        "\n"
        "-v means verbose output\n"
        "-L means read whole lines as words\n";

   exit(0);
}


using Hash = HashTable<const char *, size_t>;
using Pair = Tuple<const char *, size_t>;


void Search(const Hash *hashtable) {
    // Search the HashTable for words read from cin.

    string word;
    while (cin >> word) {
        const Pair *p = hashtable->Find(word.c_str());
        cout << (p ? p->value : 0) << "   " << word << endl;
    }
}


int main( int argc, char **argv )
  {
  if ( argc < 2 )
     Usage( );

  vector< string > words;

  CollectWordsIn( argc, argv, words );

  if ( optVerbose )
     {
     cout << "Building HashTable" << endl;

     Timer time;
     time.Start( );

     Hash *hashtable = BuildHashTable( words );

     time.Finish( );
     time.PrintElapsed( );

     cout << "Optimizing HashTable" << endl;

     time.Start( );

     hashtable->Optimize( );

     time.Finish( );
     time.PrintElapsed( );

     time.Start( );

     cout << "Enter search words:" << endl;

     Search( hashtable );

     time.Finish( );
     time.PrintElapsed( );

     delete hashtable;
     }
  else
     {
     Hash *hashtable = BuildHashTable( words );
     hashtable->Optimize( );
     Search( hashtable );
     delete hashtable;
     }
  }
