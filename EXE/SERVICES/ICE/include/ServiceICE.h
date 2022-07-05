#include <cstdlib>
#include <iostream>

#pragma once
#include "Poco/Util/ServerApplication.h"
#include "Poco/Logger.h"
#include "Poco/Channel.h"
#include "Poco/FileChannel.h"
#include "Poco/AsyncChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/PatternFormatter.h"
#include "ApplicationContext.h"
#include "Poco/Net/HTTPServer.h"

#include "ImsThread.h"
#include "KinetThread.h"
#include "RepositoryThread.h"
#include "RepositoryCommandThread.h"
#include "KinetDiscoveryThread.h"
#include "CommandDispatcherThread.h"
#include "ContextThread.h"
#include "RendererThread.h"
#include "ImsCmdThread.h"
#include "DownloadThread.h"

class ServiceICE : public Poco::Util::ServerApplication
{

public:
    ServiceICE();
    ~ServiceICE();

protected:

    void initialize(Application& self);
    void reinitialize(Application& self);
    void uninitialize();

    void initLogger();
    bool initLibs();
    bool initThreads();
    void stopService();

    int main(const std::vector<std::string> &args);

private:

    std::string pathConfig;
    std::string pathLog;
    Poco::Message::Priority priority;

    // -- threads
    ImsThread* imsThread;
    ImsCmdThread* imsCmdThread;
    KinetThread* kinetThread;
    KinetDiscoveryThread* discoveryThread;
    RepositoryThread* repoThread;
    DownloadThread* downloadThread;
    RepositoryCommandThread* repoCmdThread;
    CommandDispatcherThread* dispatcherCmdThread;
    ContextThread* contextThread;
    RendererThread* renderThread;
};

POCO_SERVER_MAIN(ServiceICE);