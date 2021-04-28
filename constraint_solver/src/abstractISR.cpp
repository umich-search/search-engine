// Abstract ISR definition.
#include<limits>
#include "Dictionary.h"
#include "abstractISR.h"

// // FOR TEST ONLY!!!
// #include<iostream>
// using namespace std;

Location max( Location locx, Location locy )
   {
     return (locx > locy)?locx:locy;
   }

ISROr::ISROr( ISR **Terms, unsigned NumberOfTerms ) 
   : Terms( Terms ), NumberOfTerms( NumberOfTerms ), 
      nearestTerm( 0 ), 
      nearestStartLocation( 0 ), nearestEndLocation( 0 ),
      currPost( 0 )
   {
   }

ISROr::~ISROr()
   {
   for ( size_t i = 0; i < NumberOfTerms; ++i )
      delete Terms[ i ];
   delete[] Terms;
   delete EndDoc;
   }

Location ISROr::GetStartLocation( )
   {
   return nearestStartLocation;
   }

Location ISROr::GetEndLocation( )
   {
   return nearestEndLocation;
   }

Post* ISROr::Seek( Location target )
   {
   // Seek all the ISRs to the first occurrence beginning at
   // the target location. Return null if there is no match.
   // The document is the document containing the nearest term.
   
   // return (start, end) of the match
   //// std::cout<< "Entered ISROr Seek" << std::endl;
   Location minLoc = SIZE_MAX;
   Post* firstPost = nullptr;
   for ( size_t i = 0; i < NumberOfTerms; ++i )
      {
      // // std::cout << "Term " << i << std::endl;
      ISR* Term = *( Terms + i ); 
      Post* nextPost = Term->Seek( target );
      // this term has no next match
      if ( !nextPost )   // there is no term after the target 
         {
          //// std::cout << "skipped" << std::endl;
         continue;
         }
      Location nextLocation = nextPost->GetStartLocation( );
      // // std::cout << nextLocation << std::endl;
      if ( nextLocation < minLoc )
         {
         //// std::cout << "hehe" << std:: endl;
         nearestTerm = i;
         delete firstPost;  // delete nullptr is allowed
         firstPost = new Post( *nextPost );
         minLoc = nextLocation;
         }
      }
  // // std::cout << nearestTerm << std::endl;
   if ( firstPost )
      {
      nearestStartLocation = firstPost->GetStartLocation( );
      nearestEndLocation = firstPost->GetEndLocation( ); 
      currPost = *firstPost;
      delete firstPost;
      //// std::cout << nearestStartLocation << " " << nearestEndLocation << std::endl;
      // // std::cout << "ISROR::Seek: return with start, end = " << firstPost->GetStartLocation( ) << ' ' << firstPost->GetEndLocation( ) << std::endl;
      return &currPost;
      }
   else
      return nullptr;
   }

Post* ISROr::Next( )
   {
   // // update nearest term
   // (*(Terms + nearestTerm))->Next();

   // Location minLoc = SIZE_MAX;
   // bool noNext = true;
   // Location termStartLocation;
   // for (unsigned i = 0; i < NumberOfTerms; ++i )
   //    {
   //    ISR* Term = *(Terms + i);
   //    // Post* nextTerm = Term->Next();
   //    termStartLocation = Term->GetStartLocation();
   //    if ( termStartLocation != SIZE_MAX )
   //       noNext = false;
   //    if ( termStartLocation < minLoc )
   //       {
   //       minLoc = termStartLocation;
   //       nearestTerm = i;
   //       }
   //    }
   // if (noNext)
   //    return nullptr;
   // // update nearest term locations (class invariants)
   // nearestStartLocation = minLoc;
   // nearestEndLocation = (*(Terms + nearestTerm))->GetEndLocation();

   // Post* nearestMatch = new Post(minLoc, nearestEndLocation);
   // return nearestMatch;
   return ISROr::Seek( nearestStartLocation + 1 );
   }

// Post* ISROr::NextDocument( )
//    {
//    // Seek all the ISRs to the first occurrence just past
//    // the end of this document.
   
//    // return (start, end) of first match

//    // TODO: DocEnd + 1 need ot be checked
//    // TODO: whether use document post or match post?
//    return Seek( EndDoc->Seek(nearestEndLocation)->GetEndLocation( ) );
//    }

 Post* ISROr::NextEndDoc( )
    {
    return nullptr;
    }

Post *ISROr::GetCurrentPost(){
    return &currPost;
}

ISR **ISROr::GetTerms( )
   {
   return this->Terms;
   }

float ISROr::GetCombinedScore( vector< float > scores )
   {
   float res = 0;
   for ( size_t i = 0; i < scores.size(); ++i )
      {
      res += scores[i];
      }
   return res;
   }

ISRAnd::ISRAnd( ISR **Terms, unsigned NumberOfTerms, Dictionary* dict ) 
   : Terms(Terms), NumberOfTerms(NumberOfTerms), currPost( 0 )
   {
   EndDoc = dict->OpenISREndDoc( );
   nearestTerm = 0;
   farthestTerm = 0;
   nearestStartLocation = 0;
   nearestEndLocation = 0;
   }

ISRAnd::~ISRAnd()
   {
   for ( size_t i = 0; i < NumberOfTerms; ++i )
      delete Terms[ i ];
   delete[] Terms;
   delete EndDoc;
   }

Location ISRAnd::GetStartLocation( )
   {
   return nearestStartLocation;
   }

Location ISRAnd::GetEndLocation( )
   {
   return nearestEndLocation;
   }

Post* ISRAnd::Seek( Location target )
   {
   // 1. Seek all the ISRs to the first occurrence beginning at
   // the target location.
   // 2. Move the document end ISR to just past the furthest
   // word, then calculate the document begin location.
   // 3. Seek all the other terms to past the document begin and the target location.
   // 4. If any term is past the document end, return to
   // step 2.
   // 5. If any ISR reaches the end, there is no match.
   //// std::cout<< "Entered ISRAnd Seek" << std::endl;
   Location minLoc = SIZE_MAX, maxLoc = 0;
   for ( size_t i = 0; i < NumberOfTerms; ++i )
      {
      ISR *Term = *( Terms + i );
      Post *nextPost = Term->Seek( target );
      // If any ISR reaches the end, there is no match.
      if ( !nextPost )
         return nullptr;
      Location nextStartLocation = nextPost->GetStartLocation( );
      Location nextEndLocation = nextPost->GetEndLocation( );
      if ( nextStartLocation < minLoc )
         {
         nearestTerm = i;
         minLoc = nextStartLocation;
         nearestStartLocation = nextStartLocation;
         nearestEndLocation = nextEndLocation;
         }
      if ( nextEndLocation > maxLoc ) // what is farthest term? when to use it?
         {
         farthestTerm = i;
         maxLoc = nextEndLocation;
         }
      }
      //// std::cout<<nearestTerm<<" " << minLoc << " " << nearestStartLocation << " " << nearestEndLocation << " " << farthestTerm << " " << maxLoc << std::endl;
   do
      {
      // 2. Move the document end ISR to just past the furthest
      // word, then calculate the document begin location.   
      // TODO: whether +1 or not, whether docStartLocation is the actual case or -1

      while (EndDoc->GetCurrentPost()->GetStartLocation() < maxLoc)
         EndDoc->Next();
      Post* endDoc = EndDoc->GetCurrentPost();
      // Post* endDoc = EndDoc->Seek( maxLoc );

      Location docEndLocation = endDoc->GetEndLocation( );
      Location docStartLocation = docEndLocation - EndDoc->GetDocumentLength( );
      //// std::cout<<docStartLocation<< "---"<<docEndLocation<<std::endl;
      minLoc = SIZE_MAX;
      maxLoc = 0;

      // 3. Seek all the other terms to past the document begin.
      bool flag = false;   // check if any term is past the enddoc
      for ( size_t i = 0; i < NumberOfTerms; ++i )
         {
         ISR *Term = *( Terms + i );

         while (Term->GetCurrentPost()->GetStartLocation() < target || Term->GetCurrentPost()->GetStartLocation() < docStartLocation)
            Term->Next();
         Post *nextPost = Term->GetCurrentPost();
         // Post *nextPost = Term->Seek( max( docStartLocation, target ) );

         // If any ISR reaches the end, there is no match.
         if ( !nextPost )
            return nullptr;
         Location nextStartLocation = nextPost->GetStartLocation( );
         Location nextEndLocation = nextPost->GetEndLocation( );
         if ( nextStartLocation < minLoc )
            {
            nearestTerm = i;
            minLoc = nextStartLocation;
            nearestStartLocation = nextStartLocation;
            nearestEndLocation = nextEndLocation;
            }
         if ( nextEndLocation > maxLoc )
            {
            farthestTerm = i;
            maxLoc = nextEndLocation;
            // 4. If any term is past the document end, return to step 2.
            if ( maxLoc > docEndLocation )
               {
               flag = true;
               break;
               }
            }
         }
      if ( flag )
         continue;
      currPost.SetLocation( minLoc, maxLoc );
      return &currPost;
      } 
      while ( true );
   }

Post* ISRAnd::Next( )
   {
   return ISRAnd::Seek( nearestStartLocation + 1 );
   }

// Post* ISRAnd::NextDocument( )
//    {
//    // Seek all the ISRs to the first occurrence just past
//    // the end of this document.

//    // return (start, end) of first match

//    // TODO: DocEnd + 1 need ot be checked
//    // TODO: whether use document post or match post?
//    return Seek( EndDoc->Seek(nearestEndLocation)->GetEndLocation( ) );
//    }

Post* ISRAnd::NextEndDoc()
   {
   return nullptr;
   }

Post *ISRAnd::GetCurrentPost(){
    return &currPost;
}

ISR **ISRAnd::GetTerms()
   {
   return this->Terms;
   }

float ISRAnd::GetCombinedScore(vector<float> scores)
   {
   float res = 1;
   for ( size_t i = 0; i < scores.size(); ++i )
      {
      res *= scores[i];
      }
   return res;
   }

ISRPhrase::ISRPhrase( ISR **Terms, unsigned NumberOfTerms ) 
   : Terms( Terms ), NumberOfTerms( NumberOfTerms ), 
   nearestStartLocation( 0 ), currPost( 0 )
   {
   }

ISRPhrase::~ISRPhrase()
   {
   for ( size_t i = 0; i < NumberOfTerms; ++i )
      delete Terms[ i ];
   delete[] Terms;
   }

Location ISRPhrase::GetStartLocation( )
   {
   return nearestStartLocation;
   }

Location ISRPhrase::GetEndLocation( )
   {
   return nearestStartLocation + NumberOfTerms - 1;
   }

Post* ISRPhrase::Seek( Location target )
   {
   // 1. Seek all ISRs to the first occurrence beginning at
   // the target location.
   // 2. Pick the furthest term and attempt to seek all
   // the other terms to the first location beginning
   // where they should appear relative to the furthest
   // term.
   // 3. If any term is past the desired location, return
   // to step 2.
   // 4. If any ISR reaches the end, there is no match.
   
   // return first match of phrase in range [ target, inf )
   // used algorithm on textbook
   do
      {
      for ( size_t i = 0; i < NumberOfTerms; ++i )
         {
         ISR *Term = *( Terms + i );
         Post* nextPost = Term->Seek( target );
         if ( !nextPost )
            return nullptr;
         else
            target = nextPost->GetStartLocation( ) + 1;
         }
      Location nextStartLocation = target - NumberOfTerms;
      bool flag = 0;
      for ( size_t i = 0; i < NumberOfTerms; ++i )
         {
         ISR *Term = *( Terms + i );
         Location desiredLocation = nextStartLocation + i;
         if ( Term->Seek( desiredLocation )->GetStartLocation( ) != desiredLocation )
            {
            target = nextStartLocation + 1;
            flag = 1;
            break;
            }
         }
      if ( flag )
         continue;
      nearestStartLocation = nextStartLocation;
      //cout<<nearestStartLocation<<' '<< nextStartLocation + NumberOfTerms - 1<<endl;
      currPost.SetLocation( nearestStartLocation, nextStartLocation + NumberOfTerms - 1 );
      return &currPost;
      } while ( true );
   }

Post* ISRPhrase::Next( )
   {
   // Finds overlapping phrase matches.
   return Seek( nearestStartLocation + 1 );
   }

Post *ISRPhrase::GetCurrentPost(){
    return &currPost;
}

// Post* ISRPhrase::NextDocument( )
//    {
//    // Seek all the ISRs to the first occurrence just past
//    // the end of this document.
   
//    // return (start, end) of first match

//    // TODO: DocEnd + 1 need ot be checked
//    // TODO: whether use document post or match post?
//    return Seek( EndDoc->Seek(nearestStartLocation)->GetEndLocation( ) );
//    }

Post* ISRPhrase::NextEndDoc()
   {
   return nullptr;
   }

ISR **ISRPhrase::GetTerms()
   {
   return this->Terms;
   }

   /*
Post* ISRContainer::Seek( Location target )
   {
   // 1. Seek all the included ISRs to the first occurrence beginning at
   // the target location.
   // 2. Move the document end ISR to just past the furthest
   // contained ISR, then calculate the document begin location.
   // 3. Seek all the other contained terms to past the document begin.
   // 4. If any contained ermis past the document end, return to
   // step 2.
   // 5. If any ISR reaches the end, there is no match.
   // 6. Seek all the excluded ISRs to the first occurrence beginning at
   // the document begin location.
   // 7. If any excluded ISR falls within the document, reset the
   // target to one past the end of the document and return to
   // step 1.
   };

Post* ISRContainer::Next( )
   {
   Seek( Contained[ nearestContained]->GetStartlocation( ) + 1 );
   }
*/
