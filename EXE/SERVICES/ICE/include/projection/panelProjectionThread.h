#include <cstdlib>
#include <iostream>
#include <queue>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "ApplicationContext.h"

class PanelProjectionThread : public Poco::Runnable
{

public:

    PanelProjectionThread(std::string panelName, int currentFrame, Template* templateExtract);
    ~PanelProjectionThread();

    bool isRunning() {return running;}

protected:

    void run();

private:

    // -- to determine projection side to do (separate left and right calculations)
    std::string panelName;
    Template* templateExtract;

    // -- to fill list of outputs
    int currentFrame;

    bool running;

};