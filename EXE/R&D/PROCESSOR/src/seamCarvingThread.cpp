#include "seamCarvingThread.h"
#include <cuda.h>

SeamCarvingThread::SeamCarvingThread(int sizeBuffer)
{
    thread = nullptr;
    state = IDLE;
    this->sizeBuffer = sizeBuffer;
}
    
SeamCarvingThread::~SeamCarvingThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}

void SeamCarvingThread::startThread()
{
    state = WORKING;
    if (thread == nullptr) {
        thread = new Poco::Thread("SeamCarving");
        thread->setPriority(Poco::Thread::Priority::PRIO_HIGHEST);
    }
    thread->start(*this);
}

void SeamCarvingThread::releaseDatas()
{
    for (int i = 0; i < in.size(); i++) {
        delete in.at(i);
    }
    state = IDLE;
}

void SeamCarvingThread::run()
{
    // -- launch processing
    int sizeWindow = 460*in.at(0)->getWidth()/2048;
    int sizeImage = in.at(0)->getWidth()+720;
    int sizeBufferIn = in.at(0)->getHeight()*in.at(0)->getWidth()*3;
    int sizeBufferOut = in.at(0)->getHeight()*sizeImage*3;

    unsigned char** datasIn = new unsigned char*[sizeBuffer];
    unsigned char** datasOut = new unsigned char*[sizeBuffer];

    Poco::Logger::get("SeamCarvingThread").debug("Copy " + std::to_string(sizeBuffer) + " images !", __FILE__, __LINE__);
    
    // -- copy all datas in one buffer
    for (int i = 0; i < sizeBuffer; i++) {
        datasIn[i] = in.at(i)->getData();
        datasOut[i] = new unsigned char[sizeBufferOut];
    }

    // -- carve seams
    Poco::Logger::get("SeamCarvingThread").debug("Launch seam carving !", __FILE__, __LINE__);
    seamResizeVideo(in.at(0)->getWidth(), in.at(0)->getHeight(), sizeImage, sizeBuffer, datasIn, datasOut);
    
    Poco::Logger::get("SeamCarvingThread").debug("Copy back images calculated !", __FILE__, __LINE__);
    for (int i = 0; i < sizeBuffer; i++) {
        unsigned char* outBuffer = new unsigned char[sizeBufferOut];
        std::memcpy(outBuffer, datasOut[i], sizeBufferOut*sizeof(unsigned char));
        Image* outImg = new Image(sizeImage, in.at(0)->getHeight(), PNG_FORMAT_RGB, outBuffer);
        out.push_back(outImg);
    }

    Poco::Logger::get("SeamCarvingThread").debug("Copy done !", __FILE__, __LINE__);
    for (int i = 0; i < sizeBuffer; i++) {
        delete[] datasOut[i];
    }
    delete[] datasIn;
    delete[] datasOut;

    state = FINISHED;
    Poco::Logger::get("SeamCarvingThread").debug("Finished !", __FILE__, __LINE__);
}