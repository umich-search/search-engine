#include <iostream>
#include <fstream>
#include "constraint_solver.h"
#include "abstractISR.h"
#include "../index/include/IndexConstructor.h"
#include "../index/include/Dictionary.h"
#include <filesystem>
using namespace std;

int main(int argc, char *argv[]) {
   ifstream docs;
   docs.open("test.txt");
   IndexConstructor ic(0);
   while (docs){
      String term;
      docs >> term;
      if (term == "#")
         break;
      if (term == "/")
         ic.Insert("cat_title", "cat.com");
      else
         ic.Insert(term, Body);
      }
   ic.FinishConstruction();
   FileManager manager = ic.fileManager;
   Dictionary dict(manager);
   char word[] = {'c', 'a', 't', '\0'};
   ISRWord word = *dict.OpenISRWord(word);

   // helper: create three ISRWord items
   ISRWord *word_quick = new ISRWord(manager, "quick\0");
   ISRWord *word_brown = new ISRWord(manager, "brown\0");
   ISRWord *word_fox = new ISRWord(manager, "fox\0");

// query 1: quick | fox
   ISR *terms_q1[] = {word_quick, word_fox};
   ISROr *q1 = new ISROr(terms_q1, 2);
   ::vector<Post*> result1 = *ConstraintSolver(&dict, q1);
   cout << "Results:" << endl;
   for (unsigned i = 0; i < result1.size(); ++i)
      {
      cout << result1[i]->GetStartLocation() << " " << result1[i]->GetEndLocation() << endl;
      }

// query 2: "quick brown quick"
  ISR *terms_q2[] = {word_quick, word_brown, word_quick};
   ISRPhrase *q2 = new ISRPhrase(terms_q2, 3);
   ConstraintSolver(q2);
   ::vector<Post*> result2 = *ConstraintSolver(&dict, q2);
   cout << "Results:" << endl;
   for (unsigned i = 0; i < result2.size(); ++i)
      {
      cout << result2[i]->GetStartLocation() << " " << result2[i]->GetEndLocation() << endl;
      }

// query 3: quick fox
   ISR *terms_q3[] = {word_quick, word_fox};
   ISRAnd *q3 = new ISRAnd(terms_q3, 2);
   ConstraintSolver(q3);
   ::vector<Post*> result3 = *ConstraintSolver(&dict, q3);
   cout << "Results:" << endl;
   for (unsigned i = 0; i < result3.size(); ++i)
      {
      cout << result3[i]->GetStartLocation() << " " << result3[i]->GetEndLocation() << endl;
      }



// query 4: quick
   ISR *terms_q4[] = {word_quick};
   ISROr *q4 = new ISROr(terms_q4, 1);
   ::vector<Post*> result4 = *ConstraintSolver(&dict, q4);
   cout << "Results:" << endl;
   for (unsigned i = 0; i < result4.size(); ++i)
      {
      cout << result4[i]->GetStartLocation() << " " << result4[i]->GetEndLocation() << endl;
      }

   delete(q1);
   delete (q2);
   delete (q3);
   delete (q4);

   delete (word_quick);
   delete (word_brown);
   delete (word_fox);

   return 0;
   }
