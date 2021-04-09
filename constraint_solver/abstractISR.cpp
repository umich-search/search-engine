// Abstract ISR definition.
#include<limits>
#include "../index/dictionary.h"
#include "abstractISR.h"

size_t min(size_t a, size_t b)
   {
   return a < b ? a : b;
   }

size_t max(size_t a, size_t b)
   {
   return a > b ? a : b;
   }

ISROr::ISROr( ISR **Terms, unsigned NumberOfTerms ) : Terms(Terms), NumberOfTerms(NumberOfTerms)
   {
   EndDoc = Dictionary::OpenISREndDoc();
   nearestTerm = 0;
   nearestStartLocation = 0;
   nearestEndLocation = 0;
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
   Location minLoc = SIZE_MAX;
   Post* firstPost = nullptr;
   for ( size_t i = 0; i < NumberOfTerms; ++i )
      {
      ISR* Term = *(Terms + i); 
      Post* nextPost = Term->Seek(target);
      // this term has no next match
      if (!nextPost)
         continue;
      Location nextLocation = nextPost->GetStartLocation();
      if (nextLocation < minLoc)
         {
         nearestTerm = i;
         firstPost = nextPost;
         minLoc = nextLocation;
         }
      }
   if (firstPost)
      {
      nearestStartLocation = firstPost->GetStartLocation();
      nearestEndLocation = firstPost->GetEndLocation(); 
      }
   return firstPost;
   }

Post* ISROr::Next( )
   {
   // // update nearest term
   // *(Terms + nearestTerm)->Next();

   // Location minLoc = SIZE_MAX;
   // // Location maxLoc = 0;
   // for (unsigned i = 0; i < NumberOfTerms; ++i )
   //    {
   //    ISR* Term = *(Terms + i);
   //    Location termStartLocation = Term->Next()->GetStartLocation();
   //    // Location nextEndLocation = Term->Next()->GetEndLocation();
   //    if ( termStartLocation < minLoc )
   //       {
   //       minLoc = termStartLocation;
   //       nearestTerm = i;
   //       }
   //    }

   // // update nearest term locations
   // nearestStartLocation = *(Terms + nearestTerm)->GetStartLocation();
   // nearestEndLocation = *(Terms + nearestTerm)->GetEndLocation();

   // Post nearestMatch = Post(minLoc);   // destruction??
   // return &nearestMatch;
   return ISROr::Seek( nearestStartLocation );
   }

Post* ISROr::NextDocument( )
   {
   // Seek all the ISRs to the first occurrence just past
   // the end of this document.
   
   // return (start, end) of first match

   // TODO: DocEnd + 1 need ot be checked
   // TODO: whether use document post or match post?
   return Seek( EndDoc->Seek(nearestEndLocation)->GetEndLocation( ) );
   }


ISRAnd::ISRAnd( ISR **Terms, unsigned NumberOfTerms ) : Terms(Terms), NumberOfTerms(NumberOfTerms) 
   {
   EndDoc = Dictionary::OpenISREndDoc();
   nearestTerm = 0;
   farthestTerm = 0;
   nearestStartLocation = 0;
   nearestEndLocation = 0;
   }

Post* ISRAnd::Seek( Location target )
   {
   // 1. Seek all the ISRs to the first occurrence beginning at
   // the target location.
   // 2. Move the document end ISR to just past the furthest
   // word, then calculate the document begin location.
   // 3. Seek all the other terms to past the document begin.
   // 4. If any term is past the document end, return to
   // step 2.
   // 5. If any ISR reaches the end, there is no match.
   Location minLoc = SIZE_MAX, maxLoc = 0;
   for (size_t i = 0; i < NumberOfTerms; ++i)
      {
      ISR *Term = *(Terms + i);
      Post *nextPost = Term->Seek(target);
      // If any ISR reaches the end, there is no match.
      if (!nextPost)
         return nullptr;
      Location nextStartLocation = nextPost->GetStartLocation();
      Location nextEndLocation = nextPost->GetEndLocation();
      if (nextStartLocation < minLoc)
         {
         nearestTerm = i;
         minLoc = nextStartLocation;
         nearestStartLocation = nextStartLocation;
         nearestEndLocation = nextEndLocation;
         }
      if (nextEndLocation > maxLoc) // what is farthest term? when to use it?
         {
         farthestTerm = i;
         maxLoc = nextEndLocation;
         }
      }
   do
      {
      // 2. Move the document end ISR to just past the furthest
      // word, then calculate the document begin location.   
      // TODO: whether +1 or not, whether docStartLocation is the actual case or -1
      EndDoc->Seek(maxLoc);
      Location docEndLocation = EndDoc->GetEndLocation();
      Location docStartLocation = docEndLocation - EndDoc->GetDocumentLength();
      Location minLoc = SIZE_MAX, maxLoc = 0;

      // 3. Seek all the other terms to past the document begin.
      for (size_t i = 0; i < NumberOfTerms; ++i)
         {
         ISR *Term = *(Terms + i);
         Post *nextPost = Term->Seek(docStartLocation);
         // If any ISR reaches the end, there is no match.
         if (!nextPost)
            return nullptr;
         Location nextStartLocation = nextPost->GetStartLocation();
         Location nextEndLocation = nextPost->GetEndLocation();
         if (nextStartLocation < minLoc)
            {
            nearestTerm = i;
            minLoc = nextStartLocation;
            nearestStartLocation = nextStartLocation;
            nearestEndLocation = nextEndLocation;
            }
         if (nextEndLocation > maxLoc)
            {
            farthestTerm = i;
            maxLoc = nextEndLocation;
            // 4. If any term is past the document end, return to step 2.
            if (maxLoc > docEndLocation)
               continue;
            }
         }
      Post nearestMatch = Post(minLoc, maxLoc);   // destruction??
      return &nearestMatch;
      } while (1);
   }

Post* ISRAnd::Next( )
   {
   return ISRAnd::Seek( nearestStartLocation );
   }

Post* ISRAnd::NextDocument( )
   {
   // Seek all the ISRs to the first occurrence just past
   // the end of this document.

   // return (start, end) of first match

   // TODO: DocEnd + 1 need ot be checked
   // TODO: whether use document post or match post?
   return Seek( EndDoc->Seek(nearestEndLocation)->GetEndLocation( ) );
   }

ISRPhrase::ISRPhrase( ISR **Terms, unsigned NumberOfTerms ) : Terms(Terms), NumberOfTerms(NumberOfTerms), nearestStartLocation(0)
   {
   EndDoc = Dictionary::OpenISREndDoc();
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
   
   // return first match of phrase in range [ target + 1, inf )
   // used algorithm on textbook
   do
      {
      for (size_t i = 0, i < NumberOfTerms; ++i)
         {
         ISR *Term = *(Terms + i);
         Post* nextPost = Term->Seek(target);
         if (!nextPost)
            return nullptr;
         else
            // TODO: whether target should +1
            target = nextPost->GetStartLocation();
         }
      Location nextStartLocation = target - NumberOfTerms;
      for (size_t i = 0, i < NumberOfTerms; ++i)
         {
         ISR *Term = *(Terms + i);
         Location desiredLocation = nextStartLocation + i + 1;
         if (Term->Seek(desiredLocation - 1)->GetStartLocation() != desiredLocation)
            {
            target = nextStartLocation + 1;
            continue;
            }
         }
      nearestStartLocation = nextStartLocation + 1;
      Post nearestMatch = Post(nearestStartLocation, nextStartLocation + NumberOfTerms);   // destruction??
      return &nearestMatch;
      } while (1);
   }

Post* ISRPhrase::Next( )
   {
   // Finds overlapping phrase matches.
   // TODO: check +1 or not?
   return Seek( nearestStartLocation );
   }

Post* ISRPhrase::NextDocument( )
   {
   // Seek all the ISRs to the first occurrence just past
   // the end of this document.
   
   // return (start, end) of first match

   // TODO: DocEnd + 1 need ot be checked
   // TODO: whether use document post or match post?
   return Seek( EndDoc->Seek(nearestStartLocation)->GetEndLocation( ) );
   }

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

