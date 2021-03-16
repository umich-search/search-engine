// HtmlParserDemo.cpp
// Nicole Hamilton, nham@umich.edu

// This is a simple demo of the HTML parser.  It reads a file
// containing presumed HTML into memory, creates an HtmlParser object
// to parse it, then prints out what it found.  By default, the output
// is a simplified HTML page representing the content.  Options allow
// for extracting only a portion of the contents.

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "HtmlParser.h"
using namespace std;

enum class Options
   {
   PrintSimplePage,
   PrintWords,
   PrintTitleWords,
   PrintLinks,
   PrintBase
   };

Options option = Options::PrintSimplePage;

void PrintParse( HtmlParser &parser )
   {
   switch ( option )
      {
      case Options::PrintSimplePage:
         // Print a simplified page

         cout << "<!DOCTYPE html>" << endl << "<html>" << endl;
         
         cout << "<title>" << endl;
         for ( auto w : parser.titleWords )
            cout << w << " ";
         cout << endl << "</title>" << endl << endl;

         cout << "<body style=\"font-family:Verdana,Arial,Helvetica,sans-serif;font-size:0.9em\">"
               << endl << "<h2>" << endl << "Body text" << endl << "</h2>" << endl;
         
         cout << "<p>" << endl;
         for ( auto w : parser.words )
            cout << w << " ";
         cout << endl << "<p>" << endl << endl;

         cout << "<h2>" << endl << "Base = " << "<a href=\"" << parser.base <<
            "\">" << parser.base << "</a>" << endl << "</h2>" << endl << endl;

         cout << "<h2>" << endl << "Links" << endl << "</h2>" << endl;
         for ( auto link : parser.links )
            {
            cout << "<p>" << endl << "<a href=\"" << link.URL << "\">" << link.URL << "</a> ( " ;
            for ( auto w : link.anchorText )
               cout << w << " ";
            cout << ")" << endl << "</p>" <<  endl;
            }

         cout << "</body>" << endl << "</html>" << endl;
         break;

      case Options::PrintWords:
         for ( auto w : parser.words )
            cout << w << endl;
         break;

      case Options::PrintTitleWords:
         for ( auto w : parser.titleWords )
            cout << w << endl;
         break;

      case Options::PrintLinks:
         for ( auto link : parser.links )
            {
            cout << link.URL << " ( ";
            for ( auto w : link.anchorText )
               cout << w << " ";
            cout << ")" << endl;
            }
         break;

      case Options::PrintBase:
         cout << parser.base << endl;
      }
   }


char *ReadFile( char *filename, size_t &fileSize )
   {
   // Read the file into memory.
   // You'll soon learn a much more efficient way to do this.

   // Attempt to Create an istream and seek to the end
   // to get the size.
   ifstream ifs( filename, ios::ate | ios::binary );
   if ( !ifs.is_open( ) )
      return nullptr;
   fileSize = ifs.tellg( );

   // Allocate a block of memory big enough to hold it.
   char *buffer = new char[ fileSize ];

   // Seek back to the beginning of the file, read it into
   // the buffer, then return the buffer.
   ifs.seekg( 0 );
   ifs.read( buffer, fileSize );
   return buffer;
   }


bool ValidOption( const char *o )
   {
   // Must be exactly one character.
   if ( !o[ 0 ] || o[ 1 ] )
      return false;

   switch ( o[ 0 ] )
      {
      case 'w':
         option = Options::PrintWords;
         break;
      case 't':
         option = Options::PrintTitleWords;
         break;
      case 'a':
         option = Options::PrintLinks;
         break;
      case 'b':
         option = Options::PrintBase;
         break;
      default:
         return false;
      }
   return true;
   }


int main( int argc, char **argv )
   {
   if ( !( argc == 2 || argc == 3 && ValidOption( argv[ 1 ] ) ) )
      {
      cout << "Usage:  HtmlParser [wtab] filename" << endl <<
              endl <<
              "By default, prints a simplified HTML page.  Options allow" << endl <<
              "for only a section of the content to be printed.  Only one" << endl <<
              "wtab option allowed at a time." << endl <<
              endl <<
              "   w  List the words, one per line." << endl <<
              "   t  List the title words, one per line." << endl << 
              "   a  list the links as URL( anchor text ), one per line." << endl <<
              "   b  Print any base that was found." << endl;
      exit( 1 );
      }

   size_t fileSize = 0;
   char *buffer = ReadFile( argv[ 1 + ( argc == 3 ) ], fileSize );
   if ( !buffer )
      {
      cerr << "Could not open the file." << endl;
      exit( 1 );
      }

   HtmlParser parser( buffer, fileSize );
   PrintParse( parser );
   delete [ ] buffer;
   }