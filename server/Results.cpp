#include "Results.h"

::vector<url_score> Results(Dictionary *dict, char* input)
   {
   ISR* queryRoot = Query_Compiler( dict, input );
   ISREndDoc* EndDoc = dict->OpenISREndDoc();
   Ranker results;
   return results.getHighest( ConstraintSolver( EndDoc, queryRoot ), queryRoot );
   }