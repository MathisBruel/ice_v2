#include <cstdlib>
#include <iostream>
#include <queue>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "ApplicationContext.h"

class MovingHeadProjectionThread : public Poco::Runnable
{

public:

    MovingHeadProjectionThread(int currentFrame, Template* extractor);
    ~MovingHeadProjectionThread();
    bool isRunning() {return running;}

protected:

    void run();

private:

    // -- to fill list of outputs
    int currentFrame;
    Template* extractor;
    bool running;

};