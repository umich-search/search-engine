#pragma once
#include <cstring>
#include "Plugin.h"
//#include "../../ranker/include/ranker.h"
//#include "../../utility/include/mString.h"
#include "../../utility/include/Concurrency.h"
#include "../../utility/include/Vector.h"
#include "RankerManager.h"

// Handle requests for the path /search?query=university+of+michigan

class SearchPlugin : public PluginObject
    {
    public:
        SearchPlugin()
            {
            //Plugin = this;
            }
        ~SearchPlugin() { };

        bool MagicPath( const std::string path )
            {
            return strncmp( path.c_str(), "/search", 7 ) == 0;
            }
        
        std::string ProcessRequest( std::string action, std::string path )
            {
            CriticalSection s(&mutex);
            if ( action != "GET" )
                return "";
            std::string query = parseQuery( path.c_str() );
            if ( query.size() == 0 )
                return "";
            std::string html;
            // TODO: fix "RankerManager.h" #include "CrawlerManager.h"
            ::vector<url_score> scores = queryServer.CollectRanks( query );
            for ( size_t i = 0; i < 3; ++i )
                {
                std::string result = "<div class=\"resultTag\"><h4><a href=\"";
                result += "https://google.com";
                //result += scores[i].URL;
                result += "\">";
                result += "Google";
                //result += scores[i].title;
                result += "</a></h4><p>";
                //result += scores[i].URL;
                result += "https://google.com";
                result += "</p></div>";
                html += result;
                }
            return html;
            }

    private:
        QueryServer queryServer;
        mutex_t mutex;

        std::string parseQuery( std::string path )
            {
            const char * search = "?query=";
            size_t start = path.find( search );
            if ( start == string::npos )
                return "";
            start += strlen( search );
            return path.substr(start);
            }
    };