#pragma once

// TopN.h
// Nicole Hamilton nham@umich.edu

// Given a hashtable, return a dynamically-allocated array
// of pointers to the top N pairs based on their values.
// Caller is responsible for deleting the array.

// Individual pairs are to be C-strings as keys and size_t's
// as values.

#include "HashTable.h"

Tuple< const char *, size_t > **TopN( HashTable< const char *,
      size_t > *hashtable, int N = 10 );