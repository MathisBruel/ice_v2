#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <mutex>

#include "png.h"

#pragma once

static const std::string CisMagicStringV1 = "CIS-V001";
static const std::string CisMagicStringV2 = "CIS-V002";
static const std::string CisMagicStringV3 = "CIS-V003";

class CisWriter
{

public:

    CisWriter();
    ~CisWriter();

    void setImageFormat(uint32_t widthIn, uint32_t heightIn, uint32_t bppIn);
    void setNbImages(int nbImagesIn);
    void addImageAsync(int frame, char* bufferImg);

    bool startSave(std::string fileName);
    bool saveFrameAsync(int frame);
    bool endSave(std::string type);

    // -- locking system
    bool tryLockMutexSaving() {return mutexSaving.try_lock();}
    void unlockMutexSaving() {mutexSaving.unlock();}

    void trySave();
    float getProgress();
    bool isFinished();

private:

    int frameRate;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;

    char** imagesBuffer;

    // -- for savings
    FILE* file;
    uint64_t* offset;
    int nbImages;
    int lastFrameSaved;

    // -- for real time optimization
    std::mutex mutexSaving;
};