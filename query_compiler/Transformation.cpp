#include <iostream>
#include "Transformation.h"

ISR* StringToISR ( Dictionary *dict, String input )
   {
   // true - AND, false - OR
   ::vector<ISR*> rootOrStream;
   ::vector<ISR*> currentAndStream;
   char *currentChar = (char*) input.begin();
   while (*currentChar != '\0')
      {
      if (*currentChar == '"')
         {
         ::vector<ISR*> currentPhraseStream;
         do
            {
            String currentWord;
            while (*(++currentChar) == ' ');
            if (*currentChar == '"')
               break;
            while ((*currentChar != ' ') && (*currentChar != '"'))
               currentWord += *(currentChar++);
            currentPhraseStream.pushBack(dict->OpenISRWord(const_cast<char *> ( currentWord.cstr())));
            } while (*currentChar != '"');
         ISR** currentPhraseISR = new ISR*[currentPhraseStream.size()];
         for (size_t i = 0; i < currentPhraseStream.size(); ++i)
            *(currentPhraseISR + i) = currentPhraseStream[i];
         currentAndStream.pushBack(new ISRPhrase(currentPhraseISR, currentPhraseStream.size()));
         while (currentPhraseStream.size())
            {
            delete *currentPhraseStream.end();
            currentPhraseStream.popBack();
            }
         }
      else if (*currentChar == '(')
         {
         String currentBracket;
         while (*(++currentChar) != ')')
            currentBracket += *currentChar;
            std::cout<<"recusion start"<<std::endl;
         currentAndStream.pushBack(StringToISR(dict, const_cast<char *> ( currentBracket.cstr())));
         std::cout<<"recursion end"<<std::endl;
         }
      else if (*currentChar == '|')
         {
         ISR** currentAndISR = new ISR*[currentAndStream.size()];
         for (unsigned i = 0; i < currentAndStream.size(); ++i)
            * (currentAndISR + i) = currentAndStream[i];
            std::cout<<"addOr" << std::endl;
         rootOrStream.pushBack(new ISRAnd(currentAndISR, currentAndStream.size(), dict));
         }
      else if ( (*currentChar != ' ') && (*currentChar != '&') )
         {
         String currentWord;
         while ( (*currentChar != ' ') && (*currentChar != '\0') )
            currentWord += *(currentChar++);
            std::cout<<"addAnd: "<< currentWord.cstr() <<std::endl;
         currentAndStream.pushBack(dict->OpenISRWord(const_cast<char *>(currentWord.cstr())));
         }
      currentChar++;
      }
   ISR** currentAndISR = new ISR*[currentAndStream.size()];
   for (unsigned i = 0; i < currentAndStream.size(); ++i)
      * (currentAndISR + i) = currentAndStream[i];

   // If there's no '|', the root type will be ISRAnd, otherwise it will be ISROr
   if (rootOrStream.size() == 0)
      {
         std::cout<<"Decision: Root is AND"<<std::endl;
         return new ISRAnd(currentAndISR, currentAndStream.size(), dict);
      }
   else
      {
         std::cout<<"Decision: Root is OR"<<std::endl;
        rootOrStream.pushBack( new ISRAnd(currentAndISR, currentAndStream.size(), dict) );
        ISR** rootOrISR = new ISR*[rootOrStream.size()];
        for (unsigned i = 0; i < rootOrStream.size(); ++i)
           * (rootOrISR + i) = rootOrStream[i];
      return new ISROr(rootOrISR, rootOrStream.size());
      }
   }
