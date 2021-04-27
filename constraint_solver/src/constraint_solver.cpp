// Constraint solver function
// Input: ISR Tree from query compiler "queryRoot"
// Output: Array of Post with (start, end) of all matching documents "posts"
#include <iostream>
#include "ISR.h"
#include "abstractISR.h"
#include "Vector.h"
#include "Dictionary.h"
#include "constraint_solver.h"

// FOR TESTING ONLY!
// using namespace std;

::vector<Match*>* ConstraintSolver(ISREndDoc* EndDoc, ISR* queryRoot)
    {
    Location currentLocation = 0;
    Post* match = nullptr;
    ::vector<Match*>* matches = new ::vector<Match*>(); 
    while ( match = queryRoot->Seek( currentLocation ) )
        {
        // find the next endDoc location in "Post endDoc"
        Post* endDoc = EndDoc->Seek(match->GetStartLocation());
        currentLocation = endDoc->GetStartLocation();
        // calculate doc length and doc start location
        Location startLocation = currentLocation - EndDoc->GetDocumentLength();
        // output index (not start, end location) of matching doc
        Match* matchDoc=new Match(EndDoc->GetCurrIndex(), startLocation, currentLocation);
        matches->pushBack(matchDoc);
        // Post* document = new Post(startLocation, currentLocation);
        // posts->pushBack(document);
        delete match;
        match = nullptr;
        }
    return matches;
    }
