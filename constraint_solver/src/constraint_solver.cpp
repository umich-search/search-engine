// Constraint solver function
// Input: ISR Tree from query compiler "queryRoot"
// Output: Array of Post with (start, end) of all matching documents "posts"
#include <iostream>
#include "../../index/include/ISR.h"
#include "../include/abstractISR.h"
#include "../../utility/include/Vector.h"
#include "../../index/include/Dictionary.h"
#include "../include/constraint_solver.h"

#define MAX_DOC 2000

using namespace std;    // FOR TESTING ONLY!

::vector<Match*>* ConstraintSolver(ISREndDoc* EndDoc, ISR* queryRoot)
   {
   Location currentLocation = 0;
   Post* match = nullptr;
   ::vector<Match*>* matches = new ::vector<Match*>(); 
   while ( match = queryRoot->Seek( currentLocation ) )
      {
      // // std::cout << "ConstraintSolver: get next query location\n";
      // find the next endDoc location in "Post endDoc"
      Location matchStart = match->GetStartLocation( );
      Post* endDoc = EndDoc->Seek( matchStart );
      currentLocation = endDoc->GetStartLocation( );
      // calculate doc length and doc start location
      Location startLocation = currentLocation - EndDoc->GetDocumentLength();
      // output index (not start, end location) of matching doc
      Match* matchDoc=new Match(EndDoc->GetCurrIndex(), startLocation, currentLocation);
      matches->pushBack(matchDoc);
      // Post* document = new Post(startLocation, currentLocation);
      // posts->pushBack(document);
      // delete match;
      match = nullptr;
      if ( matches->size() >= MAX_DOC )
         return matches;
      }
   return matches;
   }
