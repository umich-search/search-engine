#include <cassert>
#include <cstring>
#include <ctype.h>
#include "HtmlTags.h"


// name points to beginning of the possible HTML tag name.
// nameEnd points to one past last character.
// Comparison is case-insensitive.
// Use a binary search.
// If the name is found in the TagsRecognized table, return
// the corresponding action.
// If the name is not found, return OrdinaryText.

using namespace std;
DesiredAction LookupPossibleTag( const char* name, const char* nameEnd )
{
    // perform binary search for name in the table of tags
    int left = 0;
    int right = NumberOfTags - 1;
    const char* start;
    start = name;
    
    while ( left <= right )
    {
        int middle = ( left + right ) / 2;
        HtmlTag htmlTag = TagsRecognized[ middle ];
        int result = 0;
        while(name != nameEnd && *htmlTag.Tag != '\0') {
            char comp = tolower(*name);
            if(comp < *htmlTag.Tag) {
                result = -1;
                break;
            }
            else if(comp > *htmlTag.Tag) {
                result = 1;
                break;
            }
            else {
                name++;
                htmlTag.Tag++;
            }
        }
        if(result == 0) {
            if(*htmlTag.Tag == '\0' && name != nameEnd) {
                result = 1;
            }
            else if(*htmlTag.Tag != '\0' && name == nameEnd){
                result = -1;
            }
        }
        name = start;
        if ( result < 0 )
            right = middle - 1;
        else if ( result > 0 )
            left = middle + 1;
        else
        {
            return htmlTag.Action;
        }
    }
    return DesiredAction::OrdinaryText;
}
