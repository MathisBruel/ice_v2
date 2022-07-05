#include <cstdlib>
#include <iostream>
#include <queue>

#pragma once
#include "Poco/Logger.h"
#include "Poco/Runnable.h"
#include "Poco/Thread.h"
#include "Poco/Mutex.h"
#include "Poco/Path.h"
#include "image.h"
#include "seamCarvingThread.h"
#include "videoHandler.h"

class SeamProcessThread : public Poco::Runnable
{

public:

    SeamProcessThread();
    ~SeamProcessThread();
    void startThread(std::string filename, std::map<int, int> sceneCuts);
    bool isFinished() {return finished;}

protected:

    void run();

private:

    // -- for thread control
	Poco::Thread* thread;

    // -- idx to follow progression 
    int64_t nbFrames;
    int64_t frameOut;
    bool finished;

    // -- filenames
    std::string fileIn;
    std::string fileOut;

    // -- working threads 
    VideoHandler* reader;
    VideoHandler* writer;
    std::map<int, int> sceneCuts;
};