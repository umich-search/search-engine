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
#include "../include/HtmlParser.h"

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

         std::cout << "<!DOCTYPE html>" << std::endl << "<html>" << std::endl;
         
         std::cout << "<title>" << std::endl;
         for ( auto w : parser.titleWords )
            std::cout << w << " ";
         std::cout << std::endl << "</title>" << std::endl << std::endl;

         std::cout << "<body style=\"font-family:Verdana,Arial,Helvetica,sans-serif;font-size:0.9em\">"
               << std::endl << "<h2>" << std::endl << "Body text" << std::endl << "</h2>" << std::endl;
         
         std::cout << "<p>" << std::endl;
         for ( auto w : parser.words )
            std::cout << w << " ";
         std::cout << std::endl << "<p>" << std::endl << std::endl;

         std::cout << "<h2>" << std::endl << "Base = " << "<a href=\"" << parser.base <<
            "\">" << parser.base << "</a>" << std::endl << "</h2>" << std::endl << std::endl;

         std::cout << "<h2>" << std::endl << "Links" << std::endl << "</h2>" << std::endl;
         for ( auto link : parser.links )
            {
            std::cout << "<p>" << std::endl << "<a href=\"" << link.URL << "\">" << link.URL << "</a> ( " ;
            for ( auto w : link.anchorText )
               std::cout << w << " ";
            std::cout << ")" << std::endl << "</p>" <<  std::endl;
            }

         std::cout << "</body>" << std::endl << "</html>" << std::endl;
         break;

      case Options::PrintWords:
         for ( auto w : parser.words )
            std::cout << w << std::endl;
         break;

      case Options::PrintTitleWords:
         for ( auto w : parser.titleWords )
            std::cout << w << std::endl;
         break;

      case Options::PrintLinks:
         for ( auto link : parser.links )
            {
            std::cout << link.URL << " ( ";
            for ( auto w : link.anchorText )
               std::cout << w << " ";
            std::cout << ")" << std::endl;
            }
         break;

      case Options::PrintBase:
         std::cout << parser.base << std::endl;
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
      std::cout << "Usage:  HtmlParser [wtab] filename" << std::endl <<
              std::endl <<
              "By default, prints a simplified HTML page.  Options allow" << std::endl <<
              "for only a section of the content to be printed.  Only one" << std::endl <<
              "wtab option allowed at a time." << std::endl <<
              std::endl <<
              "   w  List the words, one per line." << std::endl <<
              "   t  List the title words, one per line." << std::endl << 
              "   a  list the links as URL( anchor text ), one per line." << std::endl <<
              "   b  Print any base that was found." << std::endl;
      exit( 1 );
      }

   size_t fileSize = 0;
   char *buffer = ReadFile( argv[ 1 + ( argc == 3 ) ], fileSize );
   if ( !buffer )
      {
      cerr << "Could not open the file." << std::endl;
      exit( 1 );
      }

   HtmlParser parser( buffer, fileSize );
   PrintParse( parser );
   delete [ ] buffer;
   }