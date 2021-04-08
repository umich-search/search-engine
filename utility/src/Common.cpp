// Common code used by the various hashing sample applications.

/*
#include <fstream>
#include <vector>
#include <string.h>
#include "HashTable.h"
#include "Common.h"

using namespace std;


// -v (verbose) command line option instantiation and
// initial value.

bool optVerbose = false;


// You may define additional helper routines here and in
// Common.h.

// Compare C-strings, return true if they are the same.

bool CompareEqual( const char *L, const char *R ) 
    {
    // Your code here.
    while (*L != '\0' || *R != '\0') 
        {
        if (*L != *R)return false;
        L += 1;
        R += 1;
        }
    return true;
    }


// fnv hashing function
/*
uint32_t fnvHash( const char *data, size_t length ) 
    {
    static const size_t FnvOffsetBasis = 146959810393466560;
    static const size_t FnvPrime = 1099511628211ul;
    size_t hash = FnvOffsetBasis;
    for (size_t i = 0; i < length; ++i) 
        {
        hash *= FnvPrime;
        hash ^= data[i];
        }
    return hash;
    }


using Hash = HashTable< const char *, size_t >;
using Pair = Tuple< const char *, size_t >;
*/

// Build a HashTable of strings and numbers of occurrences, given a vector
// of strings representing the words.  You may assume the vector and the
// strings will not be changed during the lifetime of the Hash.

// You may add additional parameters but they must have defaults in
// Common.h.

// Caller is responsible for deleting the Hash.
/*
Hash *BuildHashTable( const vector< string > &words ) 
    {
    // Your code here.
    Hash *hash = new Hash( );
    for ( const string &word : words ) 
        {
        Pair *target = hash->Find( word.c_str( ), 0 );
        ( target->value )++;
        }

    return hash;
    }


// Collect words read from a file specified on the command line
// as either individual word or whole lines in a vector of
// strings.

void CollectWordsIn(int argc, char **argv, vector<string> &words) {
    bool optLines = false;

    assert(argc > 1);

    argc--, argv++;
    while (**argv == '-') {
        for (char *p = *argv; *++p;)
            switch (*p) {
                case 'L':
                    optLines = true;
                    break;
                case 'v':
                case 'V':
                    optVerbose = true;
            }
        argc--, argv++;
    }

    // Open wordsin.txt

    ifstream wordsin(*argv);
    argc--;

    assert(wordsin.is_open());

    string word;
    size_t sumLengths = 0;

    if (optLines)
        while (!wordsin.eof()) {
            getline(wordsin, word);
            if (word != "") {
                words.push_back(word);
                sumLengths += word.size();
            }
        }
    else
        while (wordsin >> word) {
            words.push_back(word);
            sumLengths += word.size();
        }

    if (optVerbose) {
        size_t numberOfTokens = words.size();
        cout << "Number of tokens = " << numberOfTokens << endl;
        cout << "Total characters = " << sumLengths << endl;
        cout << "Average token length = " <<
             (double) sumLengths / (double) numberOfTokens << " characters" <<
             endl << endl;
    }
}
*/
