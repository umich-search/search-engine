#include "Results.h"

::vector<url_score> Results(Dictionary *dict, char* input)
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
   ::vector< Match * > *matches = ConstraintSolver( EndDoc, queryRoot );
   ::vector<url_score> res = results.getHighest( matches, queryRoot );
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