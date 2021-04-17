#include <iostream>
#include <query_compiler.h>

char* Normalize (char* input)
   {
   //TODO
   return input;
   }

ISR* Query_Compiler (Dictionary *dict, char* input)
   {
   input = Normalize (input);
   return StringToISR(dict, input);
   }