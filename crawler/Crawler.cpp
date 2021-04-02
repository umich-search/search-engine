#include "Crawler.h"

void Crawler::setParameters( size_t crawlerId, Frontier *front )
    {
    id = crawlerId;
    frontier = front;
    }

void Crawler::Work( )
    {
    // TODO: race condition: after all threads isDead() == true
    // two threads can check empty() == false, when PQ size == 1
    // and then call to GetUrl() will add 10 new URLs to the frontier
    while( !isDead() || !frontier->empty() )
        {
        // 1. Get a URL from the frontier
        String url = frontier->GetUrl( );

        // 2. Check for robots.txt for this domain
        ParsedUrl parsedUrl( url.cstr() );

        // 3. Retrieve the HTML webpage from the URL
        String html = LinuxGetHTML( parsedUrl );

        // 4. Parse the HTML for the webpage
        HtmlParser htmlparser( html.cstr(), html.size() );

        // 5. Add the parsed links to the frontier
        addLinksToFrontier( htmlparser );

        // 6. Add the words from the HTML to the index
        addWordsToIndex( htmlparser );
        }
    }

void Crawler::parseRobot( const String& robotUrl )
    {
    ParsedUrl parsedUrl( robotUrl.cstr() );
    parsedUrl.Path = "robots.txt";
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
                while ( robotFile[i] != '\n' && robotFile[i] != '\r' ) temp += robotFile[i++];
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
            while ( robotFile[i] != '\n' && robotFile[i] != '\r') temp += robotFile[i++];
            disallowedUrl->insert(rootUrl + temp);
            //myfile << rootUrl + temp << '\n';
            temp = "";
            continue;
            }
        else if (temp == "User-agent") break; // finished parsing User-Agent='*'
        temp += robotFile[i];
        }
    }

void Crawler::addLinksToFrontier( const HtmlParser& htmlparser )
    {
    for ( const Link& link : htmlparser.links )
        {
        // TODO: check if link has been seen
        bool linkSeen = false;
        if ( !linkSeen )
            {
            // TODO: mark the link as seen
            frontier->PushUrl( link );
            }
        }
    }
