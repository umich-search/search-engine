// Given a hashtable, find the top 10 elements.

// Nicole Hamilton  nham@umich.edu

#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include "HashTable.h"
#include "Common.h"
#include "Timer.h"
#include "TopN.h"

using namespace std;


void Usage( )
   {
   cout <<

      "Usage:  Top10 [ -vL ] wordsin.txt\n"
      "\n"
      "Builds a hash of all the words read from words.txt and then\n"
      "reports the top 10 most frequent words.\n"
      "\n"
      "-v means verbose output\n"
      "-L means read whole lines as words\n";

   exit( 0 );
   }


using Hash = HashTable< const char *, size_t >;
using Pair = Tuple< const char *, size_t >;


void PrintTop10( Hash *hashtable )
   {
   Pair **top10, *p;

   if ( optVerbose )
      {
      Timer time;

      cout << "Finding top 10" << endl;

      time.Start( );
      top10 = TopN( hashtable, 10 );
      time.Finish( );

      time.PrintElapsed( );
      }
   else
      top10 = TopN( hashtable, 10 );

   // Print the top 10.

   for ( int i = 0;  i < 10 && ( p = top10[ i ] );  i++ )
      cout << p->value << "   " << p->key << endl;

   delete [ ] top10;
   }


int main( int argc, char **argv )
   {
   if ( argc < 2 )
      Usage( );

   vector< string > words;

   CollectWordsIn( argc, argv, words );

   Hash *hashtable = BuildHashTable( words );
   hashtable->Optimize( );

   PrintTop10( hashtable );

   delete hashtable;
   }