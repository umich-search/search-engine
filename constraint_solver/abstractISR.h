// Abstract ISR declaration.
#include "ISR.h"
#include "Post.h"
#include "PostingList.h"
#pragma once

class AbstractISR: public ISR
   {
   public:
      //Get post of next matching document
      virtual Post* NextDocument( ) = 0;
   }

class ISROr: public AbstractISR
   {
   public:
      ISR **Terms;
      unsigned NumberOfTerms;
      ISREndDoc *EndDoc;
      ISROr( ISR **Terms, unsigned NumberOfTerms );

      Location GetStartLocation( );
         
      Location GetEndLocation( );
      // Seek all the ISRs to the first occurrence beginning at
      // the target location. Return null if there is no match.
      // The document is the document containing the nearest term.
      Post *Seek( Location target );
      // Do a next on the nearest term, then return
      // the new nearest match.
      Post *Next( );
      // Seek all the ISRs to the first occurrence just past
      // the end of this document.
      Post *NextDocument( );


   private:
      unsigned nearestTerm;
      // nearStartLocation and nearestEndLocation are
      // the start and end of the nearestTerm.
      Location nearestStartLocation, nearestEndLocation;
   };

class ISRAnd: public AbstractISR
   {
   public:
      ISR **Terms;
      unsigned NumberOfTerms;
      ISREndDoc *EndDoc;

      ISRAnd( ISR **Terms, unsigned NumberOfTerms );

      Post *Seek( Location target );

      Post *Next( );
      // Seek all the ISRs to the first occurrence just past
      // the end of this document.
      Post *NextDocument( );

   private:
      unsigned nearestTerm, farthestTerm;
      Location nearestStartLocation, nearestEndLocation;
   };

class ISRPhrase: public AbstractISR
   {
   public:
      ISR **Terms;
      unsigned NumberOfTerms;

      ISRPhrase( ISR **Terms, unsigned NumberOfTerms );

      Post *Seek( Location target );

      Post *Next( );

      Post *NextDocument( );

   private:
      unsigned nearestTerm, farthestTerm;
      Location nearestStartLocation;
   };

class ISRContainer: public AbstractISR
   {
   public:
      ISR **Contained, *Excluded;
      ISREndDoc *EndDoc;
      unsigned CountContained, CountExcluded;
      Location Next( );
      
      Post *Seek( Location target );

      Post *Next( );

   private:
      unsigned nearestTerm, farthestTerm;
      Location nearestStartLocation, nearestEndLocation;
   };