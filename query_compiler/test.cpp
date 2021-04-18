#include <iostream>
#include <string>
#include <cstring>
#include <query_Compiler.h>
#include <../constraint_solver/constraint_solver.h>

int main(){
   std::string input;
   while (true)
      {
      std::getline(std::cin, input);
      char *cinput = new char[input.length() + 1];
      std::strcpy(cinput, input.c_str());

      // test Normalization
      std::cout << test_String(cinput) << "\n";

      // test ISR
      Dictionary dict(0);
      ISR* query_root = Query_Compiler(&dict, cinput);


      ISREndDoc *EndDoc = dict.OpenISREndDoc();
      ::vector<Match*>* result_ptr = nullptr;
      result_ptr = ConstraintSolver(EndDoc, query_root);

      ::vector<Match*> result = *result_ptr;
      for (unsigned i = 0; i < result.size(); ++i)
         {
         std::cout << result[i]->id << ' ' << result[i]->start << ' ' << result[i]->end << std::endl;
         delete result[i];
         }
      delete result_ptr;

      // output result


      delete[] cinput;
      }
   return 0;
   }