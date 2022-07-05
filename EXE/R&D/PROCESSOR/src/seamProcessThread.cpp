#include "seamProcessThread.h"

SeamProcessThread::SeamProcessThread() 
{
    thread = nullptr;
    reader = new VideoHandler();
    writer = new VideoHandler();
}
SeamProcessThread::~SeamProcessThread() 
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
    if (reader != nullptr) {
        delete reader;
    }
    if (writer != nullptr) {
        delete writer;
    }
}

void SeamProcessThread::startThread(std::string filename, std::map<int, int> sceneCuts)
{
    nbFrames = -1;
    this->sceneCuts = sceneCuts;
    this->fileIn = filename;
    Poco::Path path(filename);
    this->fileOut = filename.replace(filename.find(path.getBaseName()), path.getBaseName().length(), path.getBaseName() + "_out");
    Poco::Logger::get("SeamProcessThread").debug("Open video file : " + this->fileIn, __FILE__, __LINE__);
    Poco::Logger::get("SeamProcessThread").debug("Open out video file : " + fileOut, __FILE__, __LINE__);
    reader->openVideo(fileIn);

    thread = new Poco::Thread("SeamProcess");
    thread->setPriority(Poco::Thread::Priority::PRIO_HIGHEST);
    thread->start(*this);
}

void SeamProcessThread::run() 
{
    while (reader->getNbFrames() == -1) {Timer::crossUsleep(1000);}
    nbFrames = reader->getNbFrames();
    Poco::Logger::get("SeamProcessThread").debug(std::to_string(nbFrames) + " to transform !", __FILE__, __LINE__);
    writer->writeVideo(nbFrames, reader->getCodecParams(), reader->getPixelFormat(), fileOut);

    // -- stop only when all frames are written to output video file
    int currentIdx = 0;
    
    bool stop = false;
    while (!stop) {

        // -- determine nb images of the scene
        std::map<int, int>::iterator it = sceneCuts.find(currentIdx);

        if (it == sceneCuts.end()) {
            stop = true;
            continue;
        }
        int sizeBuffer = it->second - currentIdx;

        Poco::Logger::get("SeamProcessThread").debug("Treat images " + std::to_string(currentIdx) + " to " + std::to_string(it->second), __FILE__, __LINE__);

        // -- get all images
        SeamCarvingThread* seamThread = new SeamCarvingThread(sizeBuffer);
        for (int i = 0; i < sizeBuffer; i++) {
            while (!reader->hasImageAtIndex(currentIdx+i)) {Timer::crossUsleep(100);}
            Image* in = reader->getImageAtIndex(currentIdx+i);
            reader->deleteImageAtIndex(currentIdx+i);
            seamThread->addImage(in);

            Poco::Logger::get("SeamProcessThread").debug("Add image " + std::to_string(currentIdx+i), __FILE__, __LINE__);
        }

        // -- start and wait for completion 
        seamThread->startThread();
        while (seamThread->getState() != SeamCarvingThread::FINISHED) {Timer::crossUsleep(100);}

        Poco::Logger::get("SeamProcessThread").debug("Seam carving terminated !", __FILE__, __LINE__);
        for (int i = 0; i < sizeBuffer; i++) {
            Image* out = seamThread->getOut(i);
            writer->addImageAtIndex(currentIdx+i, out);
        }
        seamThread->releaseDatas();
        Poco::Logger::get("SeamProcessThread").debug("Copy to writer done !", __FILE__, __LINE__);

        currentIdx += sizeBuffer;
        delete seamThread;
    }

    while (!writer->isFinished()) {Timer::crossUsleep(100);}

    Poco::Logger::get("SeamProcessThread").debug("All conversion terminated !", __FILE__, __LINE__);
}