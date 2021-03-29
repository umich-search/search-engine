// Map a HashBlob into memory and then search it for words
// read from cin.

// Nicole Hamilton  nham@umich.edu

#include <cassert>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

#include "HashBlob.h"
#include "Timer.h"

using namespace std;

extern bool optVerbose;


void Usage( )
   {
   cout <<

      "Usage:  HashFile [ -v ] blobfile\n"
      "\n"
      "Map the blobfile into memory and then search it for words\n"
      "read from cin.\n"
      "-v means verbose output\n";

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

   if ( *argv[ 0 ] == '-' )
      {
      switch ( *argv[ 1 ] )
         {
         case 'v':
         case 'V':
            optVerbose = true;
         }
      argc--, argv++;
      }

   if ( optVerbose )
      {
      cout << "Loading HashBlob from " << argv[ 1 ] << endl;

      Timer time;

      time.Start( );

      HashFile hashfile( argv[ 1 ] );

      time.Finish( );
      time.PrintElapsed( );

      const HashBlob *hashblob = hashfile.Blob( );
      cout << "HashBlob size = " << hashblob->BlobSize << " bytes" << endl;

      cout << endl << "Enter search words:" << endl;

      time.Start( );

      Search( hashblob );

      time.Finish( );
      time.PrintElapsed( );
      }
   else
      {
      HashFile hashfile( argv[ 1 ] );
      const HashBlob *hashblob = hashfile.Blob( );
      Search( hashblob );
      }
   }