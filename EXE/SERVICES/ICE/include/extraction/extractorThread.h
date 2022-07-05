#include <cstdlib>
#include <iostream>
#include <queue>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "ApplicationContext.h"

class ExtractorThread : public Poco::Runnable
{

public:

    enum TypeExtraction {
        MOVING_HEAD,
        BACK_PAR,
        PANEL_BACK_LEFT,
        PANEL_BACK_RIGHT,
        PANEL_FORE_LEFT,
        PANEL_FORE_RIGHT
    };

    ExtractorThread(TypeExtraction type, Template* extractor);
    ~ExtractorThread();
    bool isRunning() {return running;}

protected:

    void run();

private:

    // -- to determine projection side to do (separate left and right calculations)
    TypeExtraction type;
    Template* extractor;
    bool running;

};