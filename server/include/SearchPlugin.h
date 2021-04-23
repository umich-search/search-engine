#pragma once
#include <cstring>
#include "Plugin.h"
#include "mString.h"
#include "ranker.h"
#include "Concurrency.h"
//#include "RankerManager.h"

// Handle requests for the path /search?query=university+of+michigan

const std::string searchBar = "
    <form action="/search" method="get">
        <label for="query">Search:</label>
        <input type="text" id="query" name="query"><br><br>
        <input type="submit" value="Submit">
    </form>";

class SearchPlugin : public PluginObject
    {
    public:
        SearchPlugin()
            {
            Plugin = this;
            }
        ~SearchPlugin();

        bool MagicPath( const std::string path )
            {
            return strncmp( path.c_str(), "search", 6 ) == 0;
            }
        
        std::string ProcessRequest( std::string action, std::string path )
            {
            CriticalSection s(&mutex);
            if ( action != "GET" )
                return "";
            String query = parseQuery( path.c_str() );
            if ( query.size() == 0 )
                return "";
            ::vector<url_score> scores;
            std::string html = searchBar;
            // TODO: fix "RankerManager.h" #include "CrawlerManager.h"
            //::vector<url_score> scores = queryServer.CollectRanks( query );
            for ( size_t i = 0; i < scores.size(); ++i )
                {
                String result = "<h4><a href=\"";
                result += scores[i].URL;
                result += "\">";
                result += scores[i].title;
                result += "</a></h4><p>"
                result += scores[i].URL;
                result += "</p>";
                html += result;
                }
            return std::string(html.cstr());
            }

    private:
        //QueryServer queryServer;
        mutex_t mutex;

        String parseQuery( std::string path )
            {
            const char * search = "?query=";
            size_t start = path.find(search);
            if ( start == string::npos )
                return "";
            size_t end = path.find( "&", start );
            if ( end == string::npos )
                return String( path.substr(start).c_str() );
            else
                return String( path.substr(start, end-start).c_str() );
            }
    };