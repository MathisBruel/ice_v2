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

#include "RepoContext.h"

class ServiceRepo : public Poco::Util::ServerApplication
{

public:
    ServiceRepo();
    ~ServiceRepo();

protected:

    void initialize(Application& self);
    void reinitialize(Application& self);
    void uninitialize();

    void initLogger();
    bool initContext();

    int main(const std::vector<std::string> &args);

private:

    std::string pathLog;
    Poco::Message::Priority priority;
};

POCO_SERVER_MAIN(ServiceRepo);