#include "Results.h"
#include "ranker.h"

#define MAX_DOC 1000

::vector<url_score> Results(Dictionary *dict, const char* input)
   {
   ISR* queryRoot = Query_Compiler( dict, input );
   ISREndDoc* EndDoc = dict->OpenISREndDoc();
   Ranker results;
   // char quick[] = {'q', 'u', 'i', 'c', 'k', '\0'};
   // char brown[] = {'b', 'r', 'o', 'w', 'n', '\0'};
   // char fox[] = {'f', 'o', 'x', '\0'};

   // ISRWord *word_quick = dict.OpenISRWord(quick);
   // ISRWord *word_brown = dict.OpenISRWord(brown);
   // ISRWord *word_fox = dict.OpenISRWord(fox);
   // ISR *terms_q1[] = {word_quick, word_fox};
   // ISROr *q1 = new ISROr(terms_q1, 2);
   Location currentLocation = 0;
   Post* match = nullptr;
   // ::vector<Match*>* matches = new ::vector<Match*>(); 
   size_t docCount = 0;
   vector<url_score> arr;
   while ( match = queryRoot->Seek( currentLocation ) )
      {
      std::cout << "-------currentLocations = " << currentLocation << std::endl;
      // // std::cout << "ConstraintSolver: get next query location\n";
      // find the next endDoc location in "Post endDoc"
      Location matchStart = match->GetStartLocation( );
      Post* endDoc = EndDoc->Seek( matchStart );
      currentLocation = endDoc->GetStartLocation( );
      // calculate doc length and doc start location
      Location startLocation = currentLocation - EndDoc->GetDocumentLength();
      // output index (not start, end location) of matching doc
      Match* document=new Match( EndDoc->GetCurrIndex( ), startLocation, currentLocation );
      // matches->pushBack( matchDoc );
      // Post* document = new Post(startLocation, currentLocation);
      // posts->pushBack(document);
      // delete match;
      match = nullptr;

      // Ranking 
      // calculate static score
      std::cout << "Results(): start calculate_static_score()\n";
      float staticScore = calculate_static_scores( document );
      std::cout << "Results(): finish calculate_static_score() with score = " << staticScore << std::endl;
      String url, title; 
      Dictionary *dict = new Dictionary( 0 );
      std::cout << "Results(): dick->GetDocumentDetails(), doc id = " << document->id << std::endl;
      DocumentDetails *docs = dict->GetDocumentDetials( document->id );
      std::cout << "Results(): finish get doc details\n";
      url = docs->url;
      title = docs->title;
      
      delete docs;
      delete dict;

      // filtering
      bool flagExist = false;
      for ( size_t i = 0; i < arr.size( ); ++i )
         {
         if ( arr[ i ].URL == std::string( url.cstr( ) ) )
            {
            flagExist = true;
            break;
            }
         }
      if ( flagExist )
         {
         continue;
         }

      std::cout << "Results(): try to get dynamic\n";
      float dynScore = getDynamic( document, queryRoot); // searching for an abstract ISR
      std::cout << "Results(): dynamic score = " << dynScore << std::endl;
      float totalScore = dynScore * dynamicWeight + staticScore * staticWeight;
      url_score newDoc( url, title, totalScore );

      // insertion sort
      if ( arr.size() == 0 ) 
         arr.pushBack( newDoc );
      else if ( arr.size() < N ) 
         {
         int j = arr.size() - 1;
         while ( j >= 0 && arr[ j ].score < totalScore )
               {
               if ( j == arr.size() - 1 ) arr.pushBack(arr[j]);
               else arr[j + 1] = arr[j];
               --j;
               }
         if ( j + 1 < arr.size() ) arr[j + 1] = newDoc;
         else arr.pushBack( newDoc );
         }
      else if ( arr.size() >= N )
         {
         int j = N - 1;
         while ( j >=0 && arr[ j ].score < totalScore )
               {
               if( j < N - 1 ) arr[ j + 1 ] = arr[ j ];
               --j;
               }
         if ( j + 1 < N ) arr[j + 1] = newDoc;
         }
      
      // enforce maximum docs
      if ( docCount++ >= MAX_DOC )
         {
         delete document;
         break;
         }
      delete document;
      document = nullptr;
      
      }
   std::cout << "Finished calculations for ranker\n" << std::endl;
   std::cout << "*************** print ranks: " << std::endl;
   printRanks( arr );
   std::cout << "**********************************************" << std::endl;
   
   std::cout << "----------------after while loop-------------------" << std::endl;
   std::cout << "----------------match size: " << arr.size() << "-------------------" << std::endl;


   // ::vector< Match * > *matches = ConstraintSolver( EndDoc, queryRoot );
   // ::vector<url_score> res = results.getHighest( matches, queryRoot );
   // for ( size_t i = 0; i < matches->size( ); ++i )
   //    {
   //    delete ( *matches )[ i ];
   //    ( *matches )[ i ] = nullptr;
   //    }
   // delete matches;
   delete queryRoot;
   delete EndDoc;
   return arr;
   }
