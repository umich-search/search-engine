#include "ranker.h"
#include "Dictionary.h"
#include "ISRSpan.h"
#include "constraint_solver.h"
#include "BloomFilter.h"
#include <fstream>


std::string serializeUrlScore( url_score *us )
    {
    // [URL]$[title]#[score]@
    return us->URL + "\r\r\n" + 
        us->title + "\r\r\r\n" + 
        std::to_string( us->score ) + "\r\r\r\r\n";
    }

void freeResults( ::vector< url_score * >& v )
    {
    while( v.size( ) > 0 )
        {
        delete v.end( );
        v.popBack( );
        }
    }

void printRanks( ::vector< url_score >& v )
    {
    // std::ofstream os;
    // os.open( "printRankOutput.txt" );
    // for ( int i = 0; i < v.size( ); ++i )
    //     os << v[ i ].URL << '\n' 
    //             << v[ i ].title << '\n' 
    //             << v[ i ].score << std::endl << std::endl;
    // os.close( );
    for ( int i = 0; i < v.size( ); ++i )
        std::cout << v[ i ].URL << '\n' 
                << v[ i ].title << '\n' 
                << v[ i ].score << std::endl << std::endl;
    }

float getDynamic(Match* document, ISR* queryRoot);

// vector<url_score> Ranker::getHighest(::vector<Match*>* matches, ISR* queryRoot)
//     {
//     // std::cout << "Ranker::getHighest: Performing calculations for ranker\n" << std::endl;
//     vector<url_score> arr;
//     for ( size_t i = 0; i < (*matches).size(); ++i ) 
//         {
//         Match *document = (*matches)[i];
//         float staticScore = calculate_static_scores( document );;
//         //if ( queryRoot->GetTermNum() == 0 ) getDynamic( document ); // only searching for a word
//         String url, title; 
//         Dictionary *dict = new Dictionary(0);
//         DocumentDetails *docs = dict->GetDocumentDetials( document->id );
//         url = docs->url;
//         title = docs->title;
        
//         delete docs;
//         delete dict;

//         // // std::cout << "Ranker::getHighest: document details retrieved\n";
        
//         // exclude malformed urls
//         if ( url[ 0 ] != 'h' )
//             continue;

//         // filtering
//         bool flagExist = false;
//         for ( size_t i = 0; i < arr.size( ); ++i )
//             {
//             if ( arr[ i ].URL == std::string( url.cstr( ) ) )
//                 {
//                 flagExist = true;
//                 break;
//                 }
//             }
//         if ( flagExist )
//             {
//             continue;
//             }

//         // std::cout << "Ranker::getHighest: try to get dynamic\n";
//         float dynScore = getDynamic( document, queryRoot); // searching for an abstract ISR
//         // std::cout << "Ranker::getHighest: dynamic score = " << dynScore << std::endl;
//         float totalScore = dynScore * dynamicWeight + staticScore * staticWeight;
//         url_score newDoc( url, title, totalScore );

//         // insertion sort
//         if ( arr.size() == 0 ) arr.pushBack( newDoc );
//         else if ( arr.size() < N ) 
//             {
//             int j = arr.size() - 1;
//             while ( j >= 0 && arr[ j ].score < totalScore )
//                 {
//                 if ( j == arr.size() - 1 ) arr.pushBack(arr[j]);
//                 else arr[j + 1] = arr[j];
//                 --j;
//                 }
//             if ( j + 1 < arr.size() ) arr[j + 1] = newDoc;
//             else arr.pushBack( newDoc );
//             }
//         else if ( arr.size() >= N )
//             {
//             int j = N - 1;
//             while ( j >=0 && arr[ j ].score < totalScore )
//                 {
//                 if( j < N - 1 ) arr[ j + 1 ] = arr[ j ];
//                 --j;
//                 }
//             if ( j + 1 < N ) arr[j + 1] = newDoc;
//             }
        
//         }
//     // // std::cout << "Finished calculations for ranker\n" << std::endl;
//     std::cout << "*************** print ranks: " << std::endl;
//     printRanks( arr );
// 	std::cout << "**********************************************" << std::endl;
//     return arr;
//     }

float getDynamic(Match* document, ISR* queryRoot)
    {
    bool abstractISRExists = false;
    for ( int i = 0; i < queryRoot->GetTermNum(); ++i )
        {
        std::cout << (*( queryRoot->GetTerms() + i ))->GetTermNum()<<std::endl;
        if ((*( queryRoot->GetTerms() + i ))->GetTermNum() > 0) 
            {
            abstractISRExists = true;
            break;
            }
        }
    // this ISR has only WordISRs, calculate score according to heuristics
    if ( !abstractISRExists ) 
        {
        std::cout << "getDynamic(): ISRWord\n";
        return queryRoot->GetHeuristicScore( document );
        }
    else 
        {
        vector<float> scores;
        for (int i = 0; i < queryRoot->GetTermNum(); ++i) 
            {
            ISR *root = *( queryRoot->GetTerms() + i );
            if ( root->GetTermNum() > 0 ) // abstract ISR
                {
                std::cout << "getDynamic(): abstract child\n";
                scores.pushBack( getDynamic(document, root) );
                }
            else // treat the ISRWord as ISRPhrase
                {
                std::cout << "getDynamic(): ISRWord child\n";
                scores.pushBack(root->GetHeuristicScore(document));
                }
            }
        return queryRoot->GetCombinedScore(scores);
        }
    }


