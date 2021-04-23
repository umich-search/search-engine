#include <iostream>
#include "Transformation.h"
#include <cctype>

ISR* StringToISR ( Dictionary *dict, String input )
   {
   //   std::cout<< "We are in string to isr!!!!" << std::endl;
  //    std::cout<< input.cstr() << std::endl;
   // true - AND, false - OR
   ::vector<ISR*> rootOrStream;
   ::vector<ISR*> currentAndStream;
   ISR** testStream = new ISR*[1];
   unsigned count = 0;
   char *currentChar = (char*) input.begin();
   while (*currentChar != '\0')
      {
      if (*currentChar == '"')
         {
         ::vector<ISR*> currentPhraseStream;
         do
            {
            String currentWord = String();
            while (isspace(*(++currentChar)));
            if (*currentChar == '"')
               break;
            while ( (!isspace(*currentChar) ) && (*currentChar != '"'))
               currentWord += *(currentChar++);
            //std::cout<<"currentWord = \""<<   currentWord.cstr() << "\"" << std::endl;
            currentPhraseStream.pushBack(dict->OpenISRWord(currentWord.cstr()));
            } while (*currentChar != '"');
         ISR** currentPhraseISR = new ISR*[currentPhraseStream.size()];
         for (size_t i = 0; i < currentPhraseStream.size(); ++i)
            *(currentPhraseISR + i) = currentPhraseStream[i];
         currentAndStream.pushBack(new ISRPhrase(currentPhraseISR, currentPhraseStream.size()));
         while (currentPhraseStream.size())
            {
            // delete *currentPhraseStream.end();
            currentPhraseStream.popBack();
            }
         }
      else if (*currentChar == '(')
         {
         String currentBracket = String();
         while (*(++currentChar) != ')')
            currentBracket += *currentChar;
            //std::cout<<"recusion start"<<std::endl;
         currentAndStream.pushBack(StringToISR(dict, currentBracket.cstr()));
         //std::cout<<"recursion end"<<std::endl;
         }
      else if (*currentChar == '|')
         {
         ISR** currentAndTerms = new ISR*[currentAndStream.size()];
         for (unsigned i = 0; i < currentAndStream.size(); ++i)
            * (currentAndTerms + i) = currentAndStream[i];
           // std::cout<<"Construction of a new ISRAnd: size = " << currentAndStream.size() << std::endl;
         ISRAnd* currentAndISR = new ISRAnd(currentAndTerms, currentAndStream.size(), dict);
         rootOrStream.pushBack(currentAndISR);
         
         //test currentAndISR
         // ISR** testTerms = currentAndISR->GetTerms();
         // int numtestTerms = currentAndISR->GetTermNum();
         // for (unsigned i = 0; i < numtestTerms; ++i)
         // {
         //    ISR* testTerm -= * ( )
         // }

         while (currentAndStream.size())
            {
            // delete *currentAndStream.end();
            currentAndStream.popBack();
            }
         }
      else if ( (*currentChar != ' ') && (*currentChar != '&') )
         {
         String currentWord = String();
         while ( (*currentChar != ' ') && (*currentChar != '\0') )
            currentWord += *(currentChar++);
            //std::cout<<"(else) currentWord = \""<<   currentWord.cstr() << "\"" << std::endl;
            ISRWord* currentWordISR = dict->OpenISRWord(currentWord.cstr());
            currentAndStream.pushBack(currentWordISR);
         }
      currentChar++;
      }
   ISR** currentAndISR = new ISR*[currentAndStream.size()];
   for (unsigned i = 0; i < currentAndStream.size(); ++i)
      * (currentAndISR + i) = currentAndStream[i];

   // If there's no '|', the root type will be ISRAnd, otherwise it will be ISROr
   if (rootOrStream.size() == 0)
      {
         //std::cout<<"Decision: Root is AND"<<std::endl;
            //std::cout<<"Construction of a new ISRAnd: size = " << currentAndStream.size() << std::endl;
         return new ISRAnd(currentAndISR, currentAndStream.size(), dict);
      }
   else
      {
         //std::cout<<"Decision: Root is OR"<<std::endl;
           // std::cout<<"Construction of a new ISRAnd: size = " << currentAndStream.size() << std::endl;
        rootOrStream.pushBack( new ISRAnd(currentAndISR, currentAndStream.size(), dict) );
        ISR** rootOrTerms = new ISR*[rootOrStream.size()];
        for (unsigned i = 0; i < rootOrStream.size(); ++i)
           * ( rootOrTerms + i ) = rootOrStream[i];
           ///std::cout<<"Construction of a new ISROr: size = " << rootOrStream.size() << std::endl;
        ISR* root =  new ISROr(rootOrTerms, rootOrStream.size());
        return root;
      }
   }
