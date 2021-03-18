#include "Crawler.h"

void Crawler::setParameters( size_t crawlerId, Frontier *front )
    {
    id = crawlerId;
    frontier = front;
    }

void Crawler::Work( )
    {
    while( !isDead() || !frontier->empty() )
        {
        // 1. Get a URL from the frontier
        Link url = frontier->GetUrl( );

        // 2. Check for robots.txt for this domain

        // 3. Retrieve the webpage from the URL
        String html = retrieveWebpage( url );

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
    String robotFile;
    String rootUrl;
    //TODO: crawl robotFile
    String temp = "";
    int i = 0;
    for (  ; i < robotFile.size(); ++i )
        { // First find the User-agent line
        if (robotFile[i++] == ':')
            {
            if (temp == "User-agent")
                {
                temp = "";
                while (isspace(robotFile[i++])) continue;
                while (robotFile[i] != '\n') temp += robotFile[i++];
                ++i; // move i after \n
                if (temp == "*" || temp == "*\r")
                    {
                    temp = "";
                    break;
                    }
                temp = "";
                }
            }
            else // reached "disallow:"
                {
                temp = "";
                while (robotFile[i++] != '\n') continue;
                }
        temp += robotFile[i];
        }
    for ( ; i < robotFile.size(); ++i )
        { // have found userAgent = *
        if (temp == "Disallow")
            {
            temp = "";
            while ( isspace(robotFile[i]) || robotFile[i] == ':' ) ++i;
            while ( robotFile[i] != '\n' && robotFile[i] != '\r') temp += robotFile[i++];
            disallowedUrl->insert(rootUrl + temp);
            }
        else if (temp == "User-agent") break; // finished parsing User-Agent='*'
        temp += robotFile[i];
        if ( robotFile[i] == '\n' || robotFile[i] == '\r' ) temp = ""; // empty temp at end of every line
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