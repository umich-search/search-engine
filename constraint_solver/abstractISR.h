// Abstract ISR declaration.
#include "../index/include/Dictionary.h"
#include "../index/include/ISR.h"
#include "../index/include/Post.h"
#include "../index/include/PostingList.h"
#pragma once
#define MAX_SIZE 18446744073709551615

// class AbstractISR: public ISR
//    {
//    public:
//       //Get post of next matching document
//       virtual Post* NextDocument( ) = 0;
//    };

class ISROr: public ISR
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
      // Post *NextDocument( );

      Post *NextEndDoc();


   private:
      unsigned nearestTerm;
      // nearStartLocation and nearestEndLocation are
      // the start and end of the nearestTerm.
      Location nearestStartLocation, nearestEndLocation;
   };

class ISRAnd: public ISR
   {
   public:
      ISR **Terms;
      unsigned NumberOfTerms;
      ISREndDoc *EndDoc;

      ISRAnd( ISR **Terms, unsigned NumberOfTerms, Dictionary* dict );

      Location GetStartLocation( );

      Location GetEndLocation( );

      Post *Seek( Location target );

      Post *Next( );
      // Seek all the ISRs to the first occurrence just past
      // the end of this document.
      // Post *NextDocument( );

      Post *NextEndDoc();

   private:
      unsigned nearestTerm, farthestTerm;
      Location nearestStartLocation, nearestEndLocation;
   };

class ISRPhrase: public ISR
   {
   public:
      ISR **Terms;
      unsigned NumberOfTerms;

      ISRPhrase( ISR **Terms, unsigned NumberOfTerms );

      Location GetStartLocation( );

      Location GetEndLocation( );

      Post *Seek( Location target );

      Post *Next( );

      // Post *NextDocument( );

      Post *NextEndDoc();

   private:
      unsigned nearestTerm, farthestTerm;
      Location nearestStartLocation;
   };

/*
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
   };*/