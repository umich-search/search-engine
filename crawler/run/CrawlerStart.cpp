#include "CrawlerApp.h"
#include "Vector.h"
#include <fstream>

int main ( int argc, char **argv )
    {
    CrawlerApp app;
    char input;
    std::cout << "Start crawling? (Y/N): ";
    std::cin >> input;
    if ( tolower( input ) == 'y' )
        app.Start();
    else
        return 1;
    input = 'n';
    while ( tolower( input ) != 'y' )
        {
        std::cout << "Stop crawling? (Y/N): ";
        std::cin >> input;
        }
    app.Stop();
    return 0;
    }