#include <iostream>
#include <fstream>
#include "constraint_solver.h"
#include "abstractISR.h"
#include "../index/include/IndexConstructor.h"
#include "../index/include/Dictionary.h"
#include "../utility/include/mString.h"
#include <filesystem>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
   // __fs::filesystem::remove_all(CHUNK_DIRECTORY);
   // __fs::filesystem::create_directory(CHUNK_DIRECTORY);

   ifstream docs;
   docs.open("testdoc.txt");
   IndexConstructor ic(0);
   String term;
   char term0[10];
   char* pos=term0;
   char charr;
   while (docs.get(charr))
      {
      if (charr == '#')
         break;
      else if (charr == '/') 
         {
         ic.Insert("cat_title", "cat.com");
         // cout<<"Inserted!\n";
         }
      else if (charr != '\t')
         *pos++ = charr;
      else if (pos != term0)
         {
         *pos = 0;
         pos = term0;
         String term(term0);
         ic.Insert(term, Body);
         // cout<<"Inserted!1\n";
         }
      }
   ic.FinishConstruction();
   FileManager manager = ic.fileManager;
   Dictionary dict(0);
   // char word[] = {'c', 'a', 't', '\0'};
   // ISRWord isrword = *dict.OpenISRWord(word);

   // helper: create three ISRWord items
   ISREndDoc *EndDoc = dict.OpenISREndDoc();
   char quick[] = {'q', 'u', 'i', 'c', 'k', '\0'};
   char brown[] = {'b', 'r', 'o', 'w', 'n', '\0'};
   char fox[] = {'f', 'o', 'x', '\0'};

   ISRWord *word_quick = dict.OpenISRWord(quick);
   ISRWord *word_brown = dict.OpenISRWord(brown);
   ISRWord *word_fox = dict.OpenISRWord(fox);

   Post* res = nullptr;
// query 1: quick | fox
   cout << "q1 Results:" << endl;
   ISR *terms_q1[] = {word_quick, word_fox};
   ISROr *q1 = new ISROr(terms_q1, 2);

   cout<<"ISROr Seek tests:"<<endl;
   for( int i = 0; i < 10; ++i )
      {
      cout << "ISROr Seek("<<2*i<<"): ";
      res = q1->Seek(2*i);
      if (res)
         cout << res->GetStartLocation() << ' ' << res->GetEndLocation() << endl;
      }
   q1->Seek(0);
   cout<<"ISROr Next tests:"<<endl;
   while ( res = q1->Next() )
      {
      cout << res->GetStartLocation() << ' ' << res->GetEndLocation() << endl;
      }

   // cout<<"constraint solver output:"<<endl;
   // ::vector<Post*> result1 = *ConstraintSolver(EndDoc, q1);
   // for (unsigned i = 0; i < result1.size(); ++i)
   //    {
   //    cout << result1[i]->GetStartLocation() << " " << result1[i]->GetEndLocation() << endl;
   //    delete result1[i];
   //    }

// query 2: "quick brown quick"
   ISR *terms_q2[] = {word_quick, word_brown, word_quick};
   ISRPhrase *q2 = new ISRPhrase(terms_q2, 3);

   cout<<"ISRPhrase Seek tests:"<<endl;
   for( int i = 0; i < 14; ++i )
      {
      cout << "ISRPhrase Seek("<<i<<"): ";
      res = q2->Seek(i);
      if (res)
         cout << res->GetStartLocation() << ' ' << res->GetEndLocation() << endl;
      delete res;
      }

   // ::vector<Post*> result2 = *ConstraintSolver(EndDoc, q2);
   // cout << "q2 Results:" << endl;
   // for (unsigned i = 0; i < result2.size(); ++i)
   //    {
   //    cout << result2[i]->GetStartLocation() << " " << result2[i]->GetEndLocation() << endl;
   //    delete result2[i];
   //    }
      

// query 3: quick fox
   ISR *terms_q3[] = {word_quick, word_fox};
   ISRAnd *q3 = new ISRAnd(terms_q3, 2, &dict);

   cout<<"ISRAnd Seek tests:"<<endl;
   for( int i = 0; i < 14; ++i )
      {
      cout << "ISRAnd Seek("<<i<<"): ";
      res = q3->Seek(i);
      if (res)
         cout << res->GetStartLocation() << ' ' << res->GetEndLocation() << endl;
      }


   
//    ::vector<Post*> result3 = *ConstraintSolver(EndDoc, q3);
//    cout << "q3 Results:" << endl;
//    for (unsigned i = 0; i < result3.size(); ++i)
//       {
//       cout << result3[i]->GetStartLocation() << " " << result3[i]->GetEndLocation() << endl;
//       delete result3[i];
//       }



// // query 4: quick
//    ISR *terms_q4[] = {word_quick};
//    ISROr *q4 = new ISROr(terms_q4, 1);
//    ::vector<Post*> result4 = *ConstraintSolver(EndDoc, q4);
//    cout << "q4 Results:" << endl;
//    for (unsigned i = 0; i < result4.size(); ++i)
//       {
//       cout << result4[i]->GetStartLocation() << " " << result4[i]->GetEndLocation() << endl;
//       delete result4[i];
//       }

// TODO: Seek tests

// TODO: Next tests


   delete(q1);
   // delete (q2);
   // delete (q3);
   // delete (q4);

   delete (word_quick);
   delete (word_brown);
   delete (word_fox);

   return 0;
   }
