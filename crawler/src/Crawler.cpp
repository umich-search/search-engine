#include "Crawler.h"

Crawler::Crawler( Init init, Frontier *frontier, FileBloomfilter *visited, 
                 SendManager *manager ) 
    : ThreadPool( init ), frontier( frontier ), visited( visited ), 
    manager( manager ), documentCount( 0 ) 
    { 
    std::ifstream file;
    file.open( DOC_COUNT_FILE );
    size_t count;
    file >> count;
    file.close();
    documentCount = count;
    }

Crawler::~Crawler( ) { }

void Crawler::DoLoop( size_t threadID )
    {
    IndexConstructor ic(threadID);
    while ( alive )
        {
        try 
            {
            Crawl( ic, threadID );
            }
        catch ( String e )
            {
            //Print(String("Exception: ") + e, threadID );
            }
        catch ( ... )
            {
            Print("Exception: uncaught", threadID);
            }
        }
    ic.resolveChunkMem();
    }

void Crawler::parseRobot( const String& robotUrl )
    {
    std::ofstream myfile;
    myfile.open ("test.txt");
    ParsedUrl parsedUrl( robotUrl.cstr() );
    parsedUrl.Path = ROBOT_FILE;

    // Print( "extract robots.txt", 0 );
    String robotFile = LinuxGetHTML( parsedUrl, 0 );
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
                while ( i < robotFile.size() && isspace( robotFile[i] ) ) ++i;
                if ( i == robotFile.size() ) break;
                while ( i < robotFile.size() && robotFile[i] != '\n' && robotFile[i] != '\r' ) 
                    temp += robotFile[i++];
                if ( temp == "*" ) // found "User-agent: *" line
                    {
                    temp = "";
                    break;
                    }
                }
            }
        if ( i < robotFile.size() )temp += robotFile[i];
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
            while ( i < robotFile.size() && ( isspace( robotFile[i] ) || robotFile[i] == ':' )) ++i;
            if ( i == robotFile.size() ) break;
            while ( i < robotFile.size() && robotFile[i] != '\n' && robotFile[i] != '\r' ) 
                temp += robotFile[i++];
            // update the bloom filter
            visited->insert(rootUrl + temp);
            myfile << rootUrl + temp << '\n';
            temp = "";
            continue;
            }
        else if (temp == "User-agent") break; // finished parsing User-Agent='*'
        if ( i  < robotFile.size() ) temp += robotFile[i];
        }
        myfile.close();
    }

void Crawler::addWordsToIndex( const HtmlParser& htmlparser, String url, IndexConstructor &indexConstructor )
{
        char title[MAX_TITLE_LENGTH];
        size_t titleLength = 0;
        //cout << "Currently " << indexConstructor.fileManager.chunksMetadata->numChunks << " chunks created" << endl;

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
        //cout << "Inserted Document!" << endl;

        return;
    }

void Crawler::Crawl( IndexConstructor &ic, size_t threadID )
    {
    while ( alive || !frontier->Empty() )
        { 
        // 1. Get a URL from the frontier
        // if alive but frontier(pq) is empty, block until the urls
        // in the disk queue refills the pq
        String url = frontier->PopUrl( alive );
        //Print(String("Popped URL from frontier: ") + url, threadID);

        // 2. check for robots.txt
        this->parseRobot( url );
        //Print(String("ParseRobot: ") + url, threadID);

        // 3. Retrieve the HTML webpage from the URL
        ParsedUrl parsedUrl( url.cstr() );
        //Print(String("ParseURL: ") + url, threadID);
        String html = LinuxGetHTML( parsedUrl, 0 );
        //Print(String("GetHTML: ") + url, threadID);

        // 4. Parse the HTML for the webpage
        HtmlParser htmlparser( html.cstr(), html.size() );
        //Print(String("HTML parsed: ") + url, threadID);

        // 5. Send the URLs found in the HTML back to the manager
        for ( size_t i = 0; i < htmlparser.links.size(); ++i )
            {
            ParsedUrl testUrl( htmlparser.links[i].URL.cstr() );
            if ( !testUrl.IsOkay() )
                continue;
            Link* newLink = new Link( htmlparser.links[i] );
            manager->PushTask( (void *) newLink, true );
            //Print(String("Pushed URL to manager: ") + newLink->URL, threadID);
            }
        //Print(String("Num URLs parsed: ") + ltos(htmlparser.links.size()), threadID);

        // 6. Add the words from the HTML to the index
        addWordsToIndex( htmlparser, url, ic );
        //Print(String("Inserted URL in index: ") + url, threadID );
        ++documentCount;
        if ( documentCount % PRINT_INTERVAL == 0 ) 
            {
            double speed = PRINT_INTERVAL / timer.ElapsedSeconds();;
            timer.Reset();
            Print(String("Document count: ") + ltos(documentCount), threadID);
            Print(String("Crawling speed ≈ ") + ltos(speed) + String(" docs/s"), threadID);
            std::ofstream file;
            file.open( DOC_COUNT_FILE );
            file << "\r" << documentCount;
            file.close();
            }

        // Test managers
        // sleep(3);
        // Link *newLink = new Link( url );
        // manager->PushTask( ( void * ) newLink, true );
        // Print( String( "Pushed url: " ) + url, threadID );
        }
    }