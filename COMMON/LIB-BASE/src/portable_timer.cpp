#include "portable_timer.h"

void Timer::crossUsleep(int micro)
{
    #ifdef __unix__

        usleep(micro);

    #elif defined(_WIN32) || defined(WIN32)

        if (micro >= 1000) {
            Poco::Thread::sleep(micro/1000);
        }

    #endif
}