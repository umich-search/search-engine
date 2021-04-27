// Abstract ISR declaration.
#include "Dictionary.h"
#include "ISR.h"
#include "Post.h"
#include "PostingList.h"
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
      ~ISROr();

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

      ISR **GetTerms();

      int GetTermNum() { return this->NumberOfTerms; }

      Weights *getWeights() { return &(this->weights); }

      float GetCombinedScore( vector<float> scores );

   private:
      unsigned nearestTerm;
      Post currPost;  // to store stack return value

      // nearStartLocation and nearestEndLocation are
      // the start and end of the nearestTerm.
      Location nearestStartLocation, nearestEndLocation;
      struct Weights weights {
        weightShortSpan: 5,
        weightOrderSpan: 2,
        weightPhrase: 2, 
        weightTopSpan: 0.5,
        weightAll: 10,
        weightMost: 10,
        weightSome: 10
    };
   };

class ISRAnd: public ISR
   {
   public:
      ISR **Terms;
      unsigned NumberOfTerms;
      ISREndDoc *EndDoc;

      ISRAnd( ISR **Terms, unsigned NumberOfTerms, Dictionary* dict );
      ~ISRAnd();

      Location GetStartLocation( );

      Location GetEndLocation( );

      Post *Seek( Location target );

      Post *Next( );
      // Seek all the ISRs to the first occurrence just past
      // the end of this document.
      // Post *NextDocument( );

      Post *NextEndDoc();

      ISR **GetTerms();

      int GetTermNum() { return this->NumberOfTerms; }

      Weights *getWeights() { return &(this->weights); }

      float GetCombinedScore( vector<float> scores );

   private:
      unsigned nearestTerm, farthestTerm;
      Location nearestStartLocation, nearestEndLocation;
      Post currPost;  // used to store stack return value

      struct Weights weights {
        weightShortSpan: 5,
        weightOrderSpan: 2,
        weightPhrase: 2, 
        weightTopSpan: 0.5,
        weightAll: 15,
        weightMost: 10,
        weightSome: 5
    };
   };

class ISRPhrase: public ISR
   {
   public:
      ISR **Terms;
      unsigned NumberOfTerms;

      ISRPhrase( ISR **Terms, unsigned NumberOfTerms );
      ~ISRPhrase();

      Location GetStartLocation( );

      Location GetEndLocation( );

      Post *Seek( Location target );

      Post *Next( );

      // Post *NextDocument( );

      Post *NextEndDoc();

      ISR **GetTerms();

      int GetTermNum() { return this->NumberOfTerms; }

      Weights *getWeights() { return &(this->weights); }

      float GetCombinedScore( vector<float> scores ) { return 0; }

   private:
      unsigned nearestTerm, farthestTerm;
      Location nearestStartLocation;
      Post currPost;  // store stack return value

      struct Weights weights {
        weightShortSpan: 1,
        weightOrderSpan: 1,
        weightPhrase: 7, 
        weightTopSpan: 0.5,
        weightAll: 15,
        weightMost: 10,
        weightSome: 5
    };
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