// Simple HashBlob application

// Nicole Hamilton  nham@umich.edu

#include <cassert>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstring>

#include "HashBlob.h"
#include "Common.h"
#include "Timer.h"

using namespace std;


void Usage( )
   {
   cout <<

      "Usage:  HashBlob [ -L ] wordsin.txt [ blobfile ]\n"
      "\n"
      "Reads words from wordsin.txt, creates a hashblob,\n"
      "then searches it for words read from cin.\n"
      "\n"
      "If blobfile is specified, it writes the hashblob\n"
      "into the blobfile.\n"
      "\n"
      "-v means verbose output\n"
      "-L means read whole lines as words\n";

   exit( 0 );
   }


using Hash = HashTable< const char *, size_t >;
using Pair = Tuple< const char *, size_t >;


void Search( const HashBlob *hashblob )
   {
   // Search the HashBlob for words read from cin.

   string word;
   while ( cin >> word )
      {
      const SerialTuple *s = hashblob->Find( word.c_str( ) );
      cout << ( s ? s->Value : 0 ) << "   " << word << endl;
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

      if ( argc < 3 )
         {
         // Build the Hasblob in memory only.

         cout << "Building HashBlob" << endl;

         time.Start( );

         HashBlob *hashblob = HashBlob::Create( hashtable );

         time.Finish( );
         time.PrintElapsed( );

         cout << "HashBlob size = " << hashblob->BlobSize <<
            " bytes" << endl;

         cout << endl << "Enter search words:" << endl;

         time.Start( );

         Search( hashblob );

         time.Finish( );
         time.PrintElapsed( );

         HashBlob::Discard( hashblob );
         }
      else
         {
         // Build the HashBlob as a BlobFile.

         cout << "Building HashFile = " << argv[ 2 ] << endl;

         time.Start( );

         HashFile hashfile( argv[ 2 ], hashtable );

         time.Finish( );
         time.PrintElapsed( );

         const HashBlob *hashblob = hashfile.Blob( );

         cout << "HashBlob size = " << hashblob->BlobSize << " bytes" << endl;

         time.Start( );

         Search( hashblob );

         time.Finish( );
         time.PrintElapsed( );
         }

      delete hashtable;
      }
   else
      {
      Hash *hashtable = BuildHashTable( words );
      hashtable->Optimize( );

      if ( argc < 3 )
         {
         // Build the Hasblob in memory only.

         HashBlob *hashblob = HashBlob::Create( hashtable );
         Search( hashblob );
         HashBlob::Discard( hashblob );
         }
      else
         {
         // Build the HashBlob as a BlobFile.

         HashFile hashfile( argv[ 2 ], hashtable );
         const HashBlob *hashblob = hashfile.Blob( );
         Search( hashblob );
         }

      delete hashtable;
      }
   }