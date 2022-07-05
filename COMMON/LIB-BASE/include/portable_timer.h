#if defined(__linux__) || defined(__CYGWIN__)
    #include <unistd.h>
#endif

#pragma once

#include "Poco/Thread.h"

class Timer {

public:
    static void crossUsleep(int micro);
};