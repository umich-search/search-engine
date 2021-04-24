#include "Results.h"

::vector<url_score> Results(Dictionary *dict, char* input)
   {
   ISR* queryRoot = Query_Compiler( dict, input );
   ISREndDoc* EndDoc = dict->OpenISREndDoc();
   Ranker results;
   ::vector< Match * > *matches = ConstraintSolver( EndDoc, queryRoot );
   ::vector<url_score> res = results.getHighest( matches, queryRoot );

   // free allocated memory
   for ( size_t i = 0; i < matches->size( ); ++i )
   {
   delete ( *matches )[ i ];
   ( *matches )[ i ] = nullptr;
   }
   delete matches;
   delete queryRoot;
   delete EndDoc;
   
   return res;
   }