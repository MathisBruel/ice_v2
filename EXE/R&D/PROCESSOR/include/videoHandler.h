#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>

#pragma once
#include "Poco/Logger.h"
#include "Poco/Runnable.h"
#include "Poco/Mutex.h"
#include "image.h"
#include "portable_timer.h"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <inttypes.h>
}

class VideoHandler : public Poco::Runnable
{

public:

    VideoHandler();
    ~VideoHandler();

    void openVideo(std::string filename);
    void writeVideo(int64_t nbFrames, AVCodecParameters* codecParams, AVPixelFormat pixelFormat, std::string filename);

    bool hasImageAtIndex(int64_t idx);
    void addImageAtIndex(int64_t idx, Image* img);
    Image* getImageAtIndex(int64_t idx);
    void deleteImageAtIndex(int64_t idx);
    
    int64_t getNbFrames() {return nbFrames;}
    AVCodecParameters* getCodecParams() {return codecParams;}
    AVPixelFormat getPixelFormat() {return pixelFormat;}
    
    bool isFinished() {return finished;}

protected:

    void run();

private:

    int writeFrame(AVFormatContext *format, AVCodecContext *codec, AVStream *stream, AVFrame *frame, AVPacket *packet);

    Poco::Thread* thread;
    bool finished;

    // -- queue to get read images or write them to file
    Image** imgs;
    int nbImages;

    std::string filename;
    bool modeOpen;
    int64_t nbFrames;
    AVPixelFormat pixelFormat;

    AVCodecParameters* codecParams;
};