#include "string.h"
#include "string.cpp"
#include <iostream>
using namespace std;

int main()
{
    String a = "1";
    String b = "2";
    cout << a + b <<endl;
    String c = a + b;
    cout << a << " " << b << " " << c;
}