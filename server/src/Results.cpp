#include "Results.h"

::vector<url_score> Results(Dictionary *dict, char* input)
   {
   std::cout << "Querying for words: " << input << std::endl;
   ISR* queryRoot = Query_Compiler( dict, input );
   ISREndDoc* EndDoc = dict->OpenISREndDoc();
   Ranker results;
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