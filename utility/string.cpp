#include "./string.h"

std::ostream &operator<<(std::ostream &os, const String &s) {
    for (size_t i = 0; i < s.size(); i++) {
        os << *(s.begin() + i);
    }
    return os;
}