// Constraint solver function
// Input: ISR Tree from query compiler "queryRoot"
// Output: Array of Post with (start, end) of all matching documents "posts"
#include <iostream>
#include "../index/include/ISR.h"
#include "AbstractISR.h"
#include "../utility/include/Vector.h"
#include "../index/include/Dictionary.h"

// FOR TESTING ONLY!
using namespace std;

::vector<Post*>* ConstraintSolver(ISREndDoc* EndDoc, ISR* queryRoot)
    {
    Location currentLocation = 0;
    Post* match = nullptr;
    ::vector<Post*>* posts = new ::vector<Post*>(); 
    while ( match = queryRoot->Seek(currentLocation) )
        {
        // find the next endDoc location in "Post endDoc"
        Post* endDoc = EndDoc->Seek(match->GetStartLocation());
        //cout<<"EndDoc start: "<<endDoc->GetStartLocation()<<" EndDoc end: "<<endDoc->GetEndLocation()<<endl;
        // update next search start location
        currentLocation = endDoc->GetStartLocation();
        // calculate doc length and doc start location
        Location startLocation = currentLocation - EndDoc->GetDocumentLength();
        //cout << " DocLength: " << EndDoc->GetDocumentLength() << " Start: " << startLocation << ", End: " << currentLocation << endl;
        // output start, end location of matching doc
        Post* document = new Post(startLocation, currentLocation);
        posts->pushBack(document);
        }
    return posts;
    }
