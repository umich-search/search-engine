// Constraint solver function
// Input: ISR Tree from query compiler "queryRoot"
// Output: Array of Post with (start, end) of all matching documents "posts"
#include <iostream>
#include "ISR.h"
#include "abstractISR.h"
#include "Vector.h"
#include "Dictionary.h"
#include "constraint_solver.h"

// using namespace std;    // FOR TESTING ONLY!

ConstraintSolver::ConstraintSolver() : currentLocation(0) { }

Match* ConstraintSolver::findMatch(ISREndDoc* EndDoc, ISR* queryRoot)
   {
   if (currentLocation == SIZE_MAX)
      return nullptr;
   Post* match = nullptr;
   match = queryRoot->Seek( currentLocation );
   if (!match)    // no more matches
      {
      currentLocation = SIZE_MAX;
      return nullptr;
      }
   // find the next endDoc location in "Post endDoc"
   Post* endDoc = EndDoc->Seek(match->GetStartLocation());
   currentLocation = endDoc->GetStartLocation();
   // output index + start/end location of matching doc
   return new Match(EndDoc->GetCurrIndex(), currentLocation - EndDoc->GetDocumentLength(), currentLocation);
   }

void ConstraintSolver::setLocation(Location location)
   {
   currentLocation = location;
   }