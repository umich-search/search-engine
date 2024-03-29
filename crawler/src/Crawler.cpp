#include "Crawler.h"

Crawler::Crawler( Init init, Frontier *frontier, FileBloomfilter *visited, 
                 SendManager *manager ) 
    : ThreadPool( init ), frontier( frontier ), visited( visited ), 
    manager( manager ), documentCount( 0 ), ic(0) 
    { 
    std::ifstream file;
    file.open( DOC_COUNT_FILE );
    file >> documentCount;
    file.close();
    MutexInit(&indexMutex, nullptr);
    MutexInit(&docCountMutex, nullptr);
    timer.Start();
    }

Crawler::~Crawler( )
    {
    ic.resolveChunkMem();
    }

void Crawler::DoLoop( size_t threadID )
    {
    while ( alive )
        {
        try 
            {
            Crawl( threadID );
            }
        catch ( String e )
            {
            //Print(String("Exception: ") + e, threadID );
            }
        catch ( ... )
            {
            //Print("Exception: uncaught", threadID);
            }
        }
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

void Crawler::addWordsToIndex( const HtmlParser& htmlparser, String url, size_t threadID )
{
        CriticalSection s(&indexMutex);
        String title;
        bool titleMaxReached = false;
        if(htmlparser.titleWords.size() > 0 && htmlparser.titleWords[0].size() > 1 && htmlparser.titleWords[0][0] != '!') {
            size_t titleTotalLength = 0;
            for(unsigned int i = 0; i < htmlparser.titleWords.size(); ++i) {
                String s = "!";
                s += htmlparser.titleWords[i];
                ic.Insert(s, Title);
                if(!titleMaxReached) {
                    if(title.size() + htmlparser.titleWords[i].size() + 2 < MAX_TITLE_LENGTH - 1) {
                        title += htmlparser.titleWords[i];
                        title += String(' ');
                    }
                    else {
                        // Maxiumum title length - (current title size + null term) - 1
                        size_t charsToAdd = MAX_TITLE_LENGTH - title.size() - 3;
                        title += String(htmlparser.titleWords[i].cstr(), charsToAdd);
                        titleMaxReached = true;
                    }
                }

            }
        }

        for(unsigned int i = 0; i < htmlparser.words.size(); ++i) {
            //std::cout << "Inserting word: " << htmlparser.words[i].cstr() << std::endl;
            ic.Insert(htmlparser.words[i], Body);
        } 
        String output = String("Inserting title:") + title;
        //Print( output, threadID );     
        //std::cout << "Title size bytes: " << title.size() + 1 << std::endl;   
        if(title.size() < MAX_TITLE_LENGTH - 1 && url.size() < MAX_URL_LENGTH - 2) {
            ic.Insert(title, url);
        }
        //cout << "Inserted Document!" << endl;

        return;
    }

void Crawler::Crawl( size_t threadID )
    {
    while ( alive || !frontier->Empty() )
        { 
        // 1. Get a URL from the frontier
        // if alive but frontier(pq) is empty, block until the urls
        // in the disk queue refills the pq
        String url;
        try
            {
            url = frontier->PopUrl( alive );
            }
        catch( ... )
            {
            continue;
            }
        
        // Print(String("Popped URL from frontier: ") + url, threadID);

        // 2. check for robots.txt
        this->parseRobot( url );
        // Print(String("ParseRobot: ") + url, threadID);

        // 3. Retrieve the HTML webpage from the URL
        ParsedUrl parsedUrl( url.cstr() );
        // Print(String("ParseURL: ") + url, threadID);
        String html;
        try 
            {
            html = LinuxGetHTML( parsedUrl, 0 );
            }
        catch ( int e )
            {
            if ( e != GET_HTML_ERROR_RETRY )
                throw e;
            if ( !visited->contains(url) )
                {
                Link link(url);
                visited->insert(url);
                frontier->PushUrl(link);
                }
            return;
            }
        // Print(String("GetHTML: ") + html, threadID);

        // 4. Parse the HTML for the webpage
        HtmlParser htmlparser( html.cstr(), html.size() );
        // Print(String("HTML parsed: ") + url, threadID);

        // 5. Send the URLs found in the HTML back to the manager
        // if base url is malformed, take the current url as base
        if ( !ParsedUrl( htmlparser.base.cstr( ) ).IsOkay( ) )
            htmlparser.base = String(parsedUrl.Service) + String("://") + String(parsedUrl.Host) + String('/');
        ParsedUrl baseUrl( htmlparser.base.cstr( ) );

        for ( size_t i = 0; i < htmlparser.links.size(); ++i )
            {
            // if the link is empty
            if ( htmlparser.links[ i ].URL.size() == 0 )
                continue;

            // parse url
            ParsedUrl testUrl( htmlparser.links[ i ].URL.cstr() );
            Link *newLink;

            // relative urls
            if ( !testUrl.IsOkay( ) )
                {
                // start by / -> append to base's domain
                if ( testUrl.CompleteUrl[ 0 ] == '/' )
                    newLink = new Link( String( baseUrl.Service ) + String( "://" ) 
                        + String( baseUrl.Host ) + String( testUrl.CompleteUrl ) );
                // not start by / -> find current directory
                else
                    {
                    // if there is a path in base url
                    if ( ( baseUrl.Path && *( baseUrl.Path ) != 0 ) 
                        || htmlparser.base[ htmlparser.base.size( ) - 1 ] == '/' )
                        {
                        // get the directory of the base url
                        ssize_t i = htmlparser.base.size( ) - 1;
                        while ( i >= 0 && htmlparser.base[ i ] != '/' ) 
                            i--;
                        // construct new url
                        String newUrl( htmlparser.base.cstr( ), i + 1 );
                        newLink = new Link( newUrl + String( testUrl.CompleteUrl ) );
                        }
                    // if there is no path
                    else
                        newLink = new Link( htmlparser.base + String( '/' ) + String( testUrl.CompleteUrl ) );
                    }
                }
            // absolute url
            else
                newLink = new Link( htmlparser.links[ i ] );

            // push to the manager
            manager->PushTask( (void *) newLink, true );

            // ParsedUrl testUrl( htmlparser.links[i].URL.cstr() );
            // Link *newLink;
            // if ( !testUrl.IsOkay() )
            //     {
            //     if ( testUrl.CompleteUrl[ 0 ] == '/' )
            //         newLink = new Link( String( parsedUrl.Service ) + String( "://" ) 
            //             + String( parsedUrl.Host ) + String( testUrl.CompleteUrl ) );
            //     else
            //         continue;
            //     }
            // else
            //     newLink = new Link( htmlparser.links[ i ] );
            // manager->PushTask( (void *) newLink, true );
            Print(String("Pushed URL to manager: ") + newLink->URL, threadID);
            }
        //Print(String("Num URLs parsed: ") + ltos(htmlparser.links.size()), threadID);

        // 6. Add the words from the HTML to the index
        addWordsToIndex( htmlparser, url, threadID );

        //Print(String("Inserted URL in index: ") + url, threadID );
        incrementDocumentCount( threadID );

        // Test managers
        // sleep(3);
        // Link *newLink = new Link( url );
        // manager->PushTask( ( void * ) newLink, true );
        // Print( String( "Pushed url: " ) + url, threadID );
        }
    }

void Crawler::incrementDocumentCount( size_t threadID )
    {
    CriticalSection s(&docCountMutex);
    ++documentCount;
    if ( documentCount % PRINT_INTERVAL == 0 ) 
        {
        double speed = PRINT_INTERVAL / timer.ElapsedSeconds();
        timer.Reset();
        Print(String("Document count: ") + ltos(documentCount), threadID);
        Print(String("Crawling speed ≈ ") + ltos(speed) + String(" docs/s"), threadID);
        std::ofstream file;
        file.open( DOC_COUNT_FILE );
        file << "\r" << documentCount;
        file.close();
        }
    }