#include "Results.h"

::vector<url_score> Results(Dictionary *dict, const char* input)
   {
   std::cout << "Querying for words: " << input << std::endl;
   ISR* queryRoot = Query_Compiler( dict, input );
   std::cout << "Aquired term at query root location: " << queryRoot->GetStartLocation() << std::endl;
   //std::cout << "Acquired term: " << queryRoot->Print() << std::endl;

   ISREndDoc* EndDoc = dict->OpenISREndDoc();
   std::cout << "Acquire end doc at location " << EndDoc->GetStartLocation() << std::endl;

   Ranker results;
   std::cout << "initialiing solver " << std::endl;
   ConstraintSolver solver( EndDoc, queryRoot );
   // ::vector< Match * > *matches = ConstraintSolver( EndDoc, queryRoot );
   ::vector< url_score > res = results.getHighest( solver, queryRoot );

   // free allocated memory
   // for ( size_t i = 0; i < matches->size( ); ++i )
   // {
   // delete ( *matches )[ i ];
   // ( *matches )[ i ] = nullptr;
   // }
   // delete matches;
   // delete queryRoot;
   // delete EndDoc;
   
   return res;
   }