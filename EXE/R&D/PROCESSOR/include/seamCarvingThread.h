#include <cstdlib>
#include <iostream>
#include <queue>

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Logger.h"
#include "Poco/Thread.h"
#include "Poco/Mutex.h"
#include "image.h"
#include "seam.cuh"
#include "portable_timer.h"

class SeamCarvingThread : public Poco::Runnable
{

public:

    enum State {
        IDLE,
        WORKING,
        FINISHED
    };

    SeamCarvingThread(int sizeBuffer);
    ~SeamCarvingThread();

    void addImage(Image* img) {in.push_back(img);}
    void startThread();
    void releaseDatas();
    State getState() {return state;}
    Image* getOut(int idx) {return out.at(idx);}

protected:

    void run();

private:

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

    State state;
    Poco::Mutex mutex;

    int sizeBuffer;

    // -- for image processing
    std::vector<Image*> in;
    std::vector<Image*> out;
};