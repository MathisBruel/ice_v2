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

#include "seamProcessor.h"
#include "processor.h"
#include "seamProcessThread.h"
#include "portable_timer.h"
#include "seam.cuh"

#include "Poco/File.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/NamedNodeMap.h"

#pragma once

class ProcessWidget :  public Poco::Util::Application
{

public:
    ProcessWidget();
    ~ProcessWidget();

protected:

    void initialize(Application& self);
    void reinitialize(Application& self);
    void uninitialize();

    virtual int main(const std::vector<std::string> &args);

private:

    void parseScene();

    std::string fileIn;
    std::string fileScene;
    std::map<int, int> sceneCuts;
};

POCO_APP_MAIN(ProcessWidget);
