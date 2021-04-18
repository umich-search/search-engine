#include <iostream>
#include <../utility/include/mString.h>
#include <query_Compiler.h>
#include <cctype>

void incorrectQuery(const char* errMessage){
   std::cerr << errMessage;
   exit(1);
   }

// REQUIRES: a c string "input" end with \0
// 1. check parentheses and quotes
// 2. check illegal usage of operators
// 3. &&/AND -> &, ||/OR -> |
// 4. quick fox -> quick & fox
// 5. space before and after | and &
char* Normalize (char* input)
   {
   const String AND_OP = " & ";
   const String OR_OP = " | ";
   char *currentChar = input;

   // check requirement 1
   unsigned bracketCount = 0;
   while (*currentChar != '\0')
      {
      if (*currentChar == '(')
         ++bracketCount;
      if (*currentChar == ')')
         {
         if (bracketCount == 0)
            incorrectQuery("Please check brackets!\n");
         else
            --bracketCount;
         }
      if (*currentChar == '\"')
         {
         while (*(++currentChar) != '\"');
         }
      if (std::isspace(*currentChar))
         *currentChar = ' ';
      ++currentChar;
      }
   if (bracketCount)
      incorrectQuery("Please check brackets and quotes!\n");

   // check requirement 3 & 5
   String output;
   currentChar = input;
   bool inPhrase = false, prevIsOp = true;   // check requirement 4
   while (*currentChar != '\0')
      {
      //++currentChar;   // pos points to the char after "currentChar" ! ! !
      if (*currentChar == '\"')
         {
         inPhrase = !inPhrase;
         prevIsOp = false;
         output.pushBack('\"');
         }
      else if (inPhrase)   //  || (currentChar!='|'&&currentChar!='&'&&currentChar!='O'&&currentChar!='A')
         output.pushBack(*currentChar);
      else  // not inPhrase
         {
         if (*currentChar == '|' && (*(currentChar + 1) != '|' || *(currentChar + 2) != '|'))
            {
            if (prevIsOp || *(currentChar + 1) == '\0')
               incorrectQuery("Illegal logical operator!\n");
            prevIsOp = true;
            output += OR_OP;
            if (*(currentChar + 1) == '|')
               ++currentChar;
            }
         else if (*currentChar == '&' && (*(currentChar + 1) != '&' || *(currentChar + 2) != '&'))
            {
            if (prevIsOp || *(currentChar + 1) == '\0')
               incorrectQuery("Illegal logical operator!\n");
            prevIsOp = true;
            output += AND_OP;
            if (*(currentChar + 1) == '&')
               ++currentChar;
            }
         else if (*currentChar == 'O' && *(currentChar + 1) == 'R' && *(currentChar + 2) == ' ')
            {
            if (prevIsOp || *(currentChar + 1) == '\0')
               incorrectQuery("Illegal logical operator!\n");
            prevIsOp = true;
            output += OR_OP;
            currentChar += 2;
            }
         else if (*currentChar == 'A' && *(currentChar + 1) == 'N' && *(currentChar + 2) == 'D' &&
                  *(currentChar + 3) == ' ')
            {
            if (prevIsOp || *(currentChar + 1) == '\0')
               incorrectQuery("Illegal logical operator!\n");
            prevIsOp = true;
            output += AND_OP;
            currentChar += 3;
            }
         else
            {
            output.pushBack(*currentChar);
            if (*currentChar != ' ')
               prevIsOp = false;
            }
         }
      currentChar++;
      }
   if (prevIsOp)
      incorrectQuery("Illegal logical operator!\n");
   std::cout << output << std::endl;
   return const_cast<char *> (output.cstr());
   }


ISR* Query_Compiler (Dictionary *dict, char* input)
   {
   return StringToISR(dict, Normalize(input));
   }


//Just for debugging
char* test_String (char * input)
   {
   return Normalize(input);
   }