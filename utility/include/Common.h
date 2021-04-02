#pragma once
// Common code used by the various hashing sample applications.

// Nicole Hamilton  nham@umich.edu

#include <vector>
#include <string>
#include "HashTable.h"


// You may define additional helper routines here and in
// Common.cpp.



// Build a HashTable of strings and numbers of occurrences, given a vector
// of strings representing the words.  You may assume the vector and the
// strings will not be changed during the lifetime of the Hash.

// You may add additional default parameters.

// Caller is responsible for deleting the Hash.

HashTable<const char *, size_t> *BuildHashTable(const vector<string> &words);


// Collect words read from a file specified on the command line
// as either individual word or whole lines in a vector of
// strings.

void CollectWordsIn(int argc, char **argv, vector<string> &words);

// -v (verbose) command line option.

extern bool optVerbose;