#include "Crawler.h"
#include <fstream>

Crawler::Crawler( Init init, Frontier *frontier, FileBloomfilter *visited, SendManager *manager ) 
    : ThreadPool( init ), frontier( frontier ), visited( visited ), manager( manager ) { }

Crawler::~Crawler( ) { 
    indexConstructor.resolveChunkMem();
}

// TODO: another thread that pushes URLs to crawler thread pool?
// Issue: Unsure of rate of crawler ratio to rate of push to thread pool
void Crawler::DoTask( Task task, size_t threadID )
    {
    while ( alive || !frontier->Empty() )
        { 
        // 1. Get a URL from the frontier
        String url = frontier->PopUrl( alive );

        // DEBUG USE
        String pt = this->alive ? "alive " : "dead ";
        pt += url;
        this->Print( pt, threadID );

        // TODO: check for robots.txt
        this->parseRobot( url );

        // Print( "robot parsed", threadID );

        // 2. Retrieve the HTML webpage from the URL
        ParsedUrl parsedUrl( url.cstr() );

        // Print( String( parsedUrl.Host ), threadID );

        String html = LinuxGetHTML( parsedUrl );

        // this->Print( html, threadID );

        // 3. Parse the HTML for the webpage
        HtmlParser htmlparser( html.cstr(), html.size() );

        // this->Print( "finish parsing html", threadID );

        // 4. Send the URLs found in the HTML back to the manager
        for ( Link& link : htmlparser.links )
            {
            Link* newLink = new Link( link );
            manager->PushTask( (void *) newLink, true );
            }

        // this->Print( "I'm here", threadID );

        // 5. Add the words from the HTML to the index
        addWordsToIndex( htmlparser, url );
        }
    }

void Crawler::parseRobot( const String& robotUrl )
    {
    std::ofstream myfile;
    myfile.open ("test.txt");
    ParsedUrl parsedUrl( robotUrl.cstr() );
    parsedUrl.Path = ROBOT_FILE;

    // Print( "extract robots.txt", 0 );
    String robotFile = LinuxGetHTML( parsedUrl );
    // Print( "finish downloads", 0 );

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
            // update the bloom filter
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

void Crawler::addWordsToIndex( const HtmlParser& htmlparser, String url )
{
        char title[MAX_TITLE_LENGTH];
        size_t titleLength = 0;
        cout << "Currently " << indexConstructor.fileManager.chunksMetadata->numChunks << " chunks created" << endl;

        for(unsigned int i = 0; i < htmlparser.titleWords.size(); ++i) {
            size_t currTitleSize = htmlparser.titleWords[i].size();
            const char * titleCstr = htmlparser.titleWords[i].cstr();
            size_t currLoc = 0;
            size_t startPos = 0;
            while(titleCstr[currLoc] == '!') {
                startPos++;
            }
            if(startPos != currTitleSize) {
                indexConstructor.Insert(titleCstr + startPos, Title);
            }
            if(titleLength + currTitleSize + 1 < MAX_TITLE_LENGTH) {
                strcat(title, htmlparser.titleWords[i].cstr());
                char gap = ' ';
                strcat(title, &gap);
                titleLength += currTitleSize + 1;
            }
        }
        for(unsigned int i = 0; i < htmlparser.words.size(); ++i) {
            indexConstructor.Insert(htmlparser.words[i], Body);
        }
        
        indexConstructor.Insert(String(title), url);
        cout << "Inserted Document!" << endl;

        return;
    }