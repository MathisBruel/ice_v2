#include <iostream>
#include <stdlib.h>

#include "Poco/Logger.h"
#include "Poco/Channel.h"
#include "Poco/FileChannel.h"
#include "Poco/AsyncChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/Thread.h"
#include "Poco/StopWatch.h"
#include "Poco/Util/Application.h"

#include "portable_timer.h"
#include "image.h"
#include "Converter.h"
#include "videoHandler.h"

#include "Poco/File.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/NamedNodeMap.h"

#pragma once

class VideoSyncer :  public Poco::Util::Application
{

public:
    VideoSyncer();
    ~VideoSyncer();

    enum TypeMatch
    {
        EQUAL,
        ADD,
        REPLACE,
        MISSING,
        UNDETERMINED
    };

    struct FrameMatch
    {
        int frameRef;
        int sizeRef;
        int frameNew;
        int sizeNew;
        TypeMatch type;
    };

protected:

    void initialize(Application& self);
    void reinitialize(Application& self);
    void uninitialize();

    virtual int main(const std::vector<std::string> &args);

private:

    void fillHashes();
    void parseScene();
    void checkFeature();

    std::string fileRef;
    std::string fileNew;
    std::string type;
    std::string fileScene;

    std::map<int, int> sceneCuts;

    std::map<std::string, std::vector<int>> mapRef;
    std::map<std::string, std::vector<int>> mapNew;
    std::map<int, std::string> mapFrameRef;
    std::map<int, std::string> mapFrameNew;
};

POCO_APP_MAIN(VideoSyncer);
