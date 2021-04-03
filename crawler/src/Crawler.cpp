#include "Crawler.h"
#include <fstream>

Crawler::Crawler( ){};

Crawler::~Crawler( ){};

void Crawler::DoTask( void *args )
    {
    // 1. Get a URL from the frontier
    String *url = (String *)args;

    // 2. Check for robots.txt for this domain
    ParsedUrl parsedUrl( url->cstr() );

    // 3. Retrieve the HTML webpage from the URL
    String html = LinuxGetHTML( parsedUrl );

    // 4. Parse the HTML for the webpage
    HtmlParser htmlparser( html.cstr(), html.size() );

    // 5. Add the parsed links to the frontier
    addLinksToFrontier( htmlparser );

    // 6. Add the words from the HTML to the index
    addWordsToIndex( htmlparser );
    }

void Crawler::parseRobot( const String& robotUrl )
    {
    std::ofstream myfile;
    myfile.open ("test.txt");
    ParsedUrl parsedUrl( robotUrl.cstr() );
    parsedUrl.Path = ROBOT_FILE;
    String robotFile = LinuxGetHTML( parsedUrl );
    String rootUrl = String(parsedUrl.Service) + String("://") + String(parsedUrl.Host);
    if ( *parsedUrl.Port ) rootUrl = rootUrl + ":" + String(parsedUrl.Port);
    String temp = "";
    int i = 0;
    for ( ; i < robotFile.size(); ++i )
        { // First find the User-agent * line, if not found, reach to EOF
        if ( robotFile[i] == '\n' || robotFile[i] == '\r' ) 
            {
            temp = "";
            continue;
            }
        if ( robotFile[i] == ':' )
            {
            if (temp == "User-agent")
                {
                temp = "";
                ++i;
                while ( isspace( robotFile[i] ) ) ++i;
                while ( robotFile[i] != '\n' && robotFile[i] != '\r' ) 
                    temp += robotFile[i++];
                if ( temp == "*" ) // found "User-agent: *" line
                    {
                    temp = "";
                    break;
                    }
                }
            }
        temp += robotFile[i];
        }
    for ( ; i < robotFile.size(); ++i )
        { // have found userAgent = *
        if ( robotFile[i] == '\n' || robotFile[i] == '\r' ) 
            {
            temp = "";
            continue;
            }
        if ( temp == "Disallow" )
            {
            temp = "";
            while ( isspace( robotFile[i] ) || robotFile[i] == ':' ) ++i;
            while ( robotFile[i] != '\n' && robotFile[i] != '\r') 
                temp += robotFile[i++];
            visited->insert(rootUrl + temp);
            myfile << rootUrl + temp << '\n';
            temp = "";
            continue;
            }
        else if (temp == "User-agent") break; // finished parsing User-Agent='*'
        temp += robotFile[i];
        }
        myfile.close();
    }

void Crawler::addLinksToFrontier( HtmlParser& htmlparser )
    {
    for ( Link& link : htmlparser.links )
        {
        bool linkSeen = visited->contains( link.URL );
        if ( !linkSeen )
            {
            // order matters! we don't mind losing a link but we mind duplicates
            visited->insert( link.URL );
            frontier->PushUrl( link );
            }
        }
    }

void Crawler::addWordsToIndex( const HtmlParser& htmlparser )
    {
    // TODO: implement this function
    return;
    }