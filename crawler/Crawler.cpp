#include "crawler.h"

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