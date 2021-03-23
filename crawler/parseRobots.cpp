#include "Crawler.h"
#include <iostream>
#include <fstream>
using namespace std;

Crawler::Crawler( ){};

Crawler::~Crawler( ){};

void Crawler::Work(){};

void Crawler::parseRobot( const String& robotUrl )
    {
    ofstream myfile;
    myfile.open ("test.txt");
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
            //TODO: finish bloom filter
            //disallowedUrl->insert(rootUrl + temp);
            myfile << rootUrl + temp << '\n';
            temp = "";
            continue;
            }
        else if (temp == "User-agent") break; // finished parsing User-Agent='*'
        temp += robotFile[i];
        }
        myfile.close();
    }


int main()
{
    Crawler myCrawler;
    myCrawler.parseRobot("https://baidu.com/robots.txt");
    return 0;
}