#include <iostream>
#include <Transformation.h>

ISR* StringToISR ( Dictionary *dict, char* input )
   {
   ::vector<ISR *> tokens;
   // true - AND, false - OR
   ::vector<bool> logical_operator;
   char *currentChar = input;
   while (*currentChar != '\0')
      {
      if (*currentChar == '"')
         {
         ISR **currentPhraseStream;
         unsigned currentPhraseSize = 0;
         do
            {
            String currentWord;
            while (*(++currentChar) == ' ');
            if (*currentChar == '"')
               break;
            while ((*currentChar != ' ') && (*currentChar != '"'))
               currentWord += *(currentChar++);
            *(currentPhraseStream + currentPhraseSize++) = dict->OpenISRWord(const_cast<char *> ( currentWord.cstr()));
            } while (*currentChar != '"');
         tokens.pushBack(new ISRPhrase(currentPhraseStream, currentPhraseSize));
         }
      else if (*currentChar == '(')
         {
         String currentBracket;
         while (*(++currentChar) != ')')
            currentBracket += *currentChar;
         tokens.pushBack(StringToISR(dict, const_cast<char *> ( currentBracket.cstr())));
         }
      else if (*currentChar == '|')
         {
         logical_operator.pushBack(false);
         }
      else if (*currentChar == '&')
         {
         logical_operator.pushBack(true);
         }
      else if (*currentChar != ' ')
         {
         String currentWord;
         while (*currentChar != ' ')
            currentWord += *(currentChar++);
         tokens.pushBack(dict->OpenISRWord(const_cast<char *>(currentWord.cstr())));
         }
      currentChar++;
      }

   // If there's no '|', the root type will be ISRAnd, otherwise it will be ISROr
   unsigned currentOperator = 0;
   ISR **rootOrStream = nullptr;
   unsigned rootOrStreamSize = 0;
   ISR **currentAndStream;
   *currentAndStream = tokens[0];
   unsigned currentAndStreamSize = 1;
   while (currentOperator < logical_operator.size())
      {
      if (!logical_operator[currentOperator])
         {
         *(rootOrStream + rootOrStreamSize++) = new ISRAnd(currentAndStream, currentAndStreamSize, dict);
         currentAndStreamSize = 0;
         }
      *(currentAndStream + currentAndStreamSize++) = tokens[currentOperator++];
      }
   if (rootOrStreamSize == 0)
      return new ISRAnd(currentAndStream, currentAndStreamSize, dict);
   else
      {
      *(rootOrStream + rootOrStreamSize++) = new ISRAnd(currentAndStream, currentAndStreamSize, dict);
      return new ISROr(rootOrStream, rootOrStreamSize);
      }
   }
