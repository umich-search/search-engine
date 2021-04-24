#include <iostream>
#include <fstream>
#include "HtmlParser.h"
#include "Vector.h"
#include "GetUrl.h"
#include "IndexConstructor.h"

const char *MACHINE_DIR[] = 
    {
    "m1/", "m2/", "m3/"
    };

unsigned int port[] = { 8000, 8001, 8002 };

int main( int argc, char *argv[ ] )
    {
    if ( argc != 2 )
        {
        std::cout << "Usage: ./tinyCrawler [machine id].\n";
        return -1;
        }

    int mID = argc;

    // get urls to crawl
    String seedFile = String( "./seedFile.txt" );
    std::ifstream sin;
    sin.open( seedFile.cstr( ) );
    String url;
    int count = 0;

    // index the pages into index files
    IndexConstructor ic( 0 );

    while ( sin >> url )
        {
        try
            {
            String completeUrl = "https://";
            completeUrl += url;
            std::cout << completeUrl << std::endl;
            // visit the internet to collect those pages
            ParsedUrl parsedUrl( completeUrl.cstr( ) );
            String html = LinuxGetHTML( parsedUrl, 0 );
            HtmlParser htmlparser( html.cstr( ), html.size( ) );
        

            String title;
            bool titleMaxReached = false;
            if( htmlparser.titleWords.size( ) > 0 && htmlparser.titleWords[ 0 ].size( ) > 1 && htmlparser.titleWords[ 0 ][ 0 ] != '!' ) 
                {
                size_t titleTotalLength = 0;
                for( unsigned int i = 0; i < htmlparser.titleWords.size( ); ++i ) 
                    {
                    String s = "!";
                    s += htmlparser.titleWords[ i ];
                    ic.Insert( s, Title );
                    if( !titleMaxReached ) 
                        {
                        if( title.size( ) + htmlparser.titleWords[ i ].size( ) + 2 < MAX_TITLE_LENGTH - 1 ) 
                            {
                            title += htmlparser.titleWords[ i ];
                            title += String( ' ' );
                            }
                        else 
                            {
                            // Maxiumum title length - (current title size + null term) - 1
                            size_t charsToAdd = MAX_TITLE_LENGTH - title.size( ) - 3;
                            title += String( htmlparser.titleWords[ i ].cstr( ), charsToAdd );
                            titleMaxReached = true;
                            }
                        }
                    }
                }
                
            // insert body
            for(unsigned int i = 0; i < htmlparser.words.size(); ++i) 
                {
                ic.Insert(htmlparser.words[i], Body);
                } 

            String output = String("Inserting title:") + title;
            std::cout << output << std::endl;

            // insert end of doc
            if( title.size() < MAX_TITLE_LENGTH - 1 && url.size() < MAX_URL_LENGTH - 2 ) 
                {
                ic.Insert( title, url );
                }
            ++count;

            // if ( count % 3 == 0 )
            //     ic.FinishConstruction();
            
            }

        catch( ... )
            {
            std::cout << "Exception catched, function continues\n";
            continue;
            }
        }
        
        ic.FinishConstruction();
    }
