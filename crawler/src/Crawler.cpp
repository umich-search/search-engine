#include "Crawler.h"
#include <fstream>

Crawler::Crawler( Init init, Frontier *frontier, FileBloomfilter *visited, SendManager *manager ) 
    : ThreadPool( init ), frontier( frontier ), visited( visited ), manager( manager ) { }

Crawler::~Crawler( ) { }

void Crawler::DoTask( Task task, size_t threadID )
    {
    while ( alive || !frontier->Empty() )
        {
        // 1. Get a URL from the frontier
        // TODO: add param to PopUrl() that decides whether to refresh PQ when empty
        String url = frontier->PopUrl( alive );

        // 2. Retrieve the HTML webpage from the URL
        ParsedUrl parsedUrl( url.cstr() );
        String html = LinuxGetHTML( parsedUrl );

        // 3. Parse the HTML for the webpage
        HtmlParser htmlparser( html.cstr(), html.size() );

        // 4. Send the URLs found in the HTML back to the manager
        for ( Link& link : htmlparser.links )
            {
            Link* newLink = new Link( link );
            manager->PushTask( (void *) newLink, true );
            }

        // 5. Add the words from the HTML to the index
        addWordsToIndex( htmlparser );
        }
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

void Crawler::addWordsToIndex( const HtmlParser& htmlparser )
    {
    // TODO: implement this function
    return;
    }