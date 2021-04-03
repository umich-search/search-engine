#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cassert>
#include <string.h>
#include "mString.h"

int main( )
    {
    const char *fname = "fuck.txt";
    int f = open( fname, O_RDWR );
    std::cerr << "errno = " << errno << ' ' << strerror( errno ) << std::endl;
    }