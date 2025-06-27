#include "ServiceCentral.h"

ServiceCentral::ServiceCentral() 
{
    pathLog = "";
}
    
ServiceCentral::~ServiceCentral()
{
    Poco::Util::Application::uninitialize();
}

void ServiceCentral::initialize(Application& self) {
    Application::initialize(self);
}
void ServiceCentral::reinitialize(Application& self) {
    Application::reinitialize(self);
}
void ServiceCentral::uninitialize() {}

void ServiceCentral::initLogger()
{
    // -- change every 50 MB and save old with timestamp and compress it 
    // -- with a maximum of 5 archived files
    if (pathLog == "") {
        pathLog = "/var/ICE2/LOG/ICE-CENTRAL.log";
        std::cout << "PathLog not setted ! Default " + pathLog + " is taken." << std::endl;
    }
    Poco::Channel* pChannel = new Poco::FileChannel();
    pChannel->setProperty("path", pathLog);
    pChannel->setProperty("rotation", "50 M");
    pChannel->setProperty("archive", "timestamp");
    pChannel->setProperty("times", "local");
    pChannel->setProperty("compress", "true");
    pChannel->setProperty("purgeCount", "5");

    // -- format output of log
    Poco::PatternFormatter* pattern_formatter = new Poco::PatternFormatter("%Y-%m-%d %H:%M:%S %U:%u %s: %t");
    Poco::FormattingChannel* formatter_channel = new Poco::FormattingChannel(pattern_formatter, pChannel);
    
    // -- initialize log as asynchronous channel (do not take too much process)
    Poco::AsyncChannel* pSync = new Poco::AsyncChannel(formatter_channel);

    try {
       Poco::Logger::root().setChannel(pSync);
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    Poco::Logger::root().setLevel(priority);
}

int ServiceCentral::main(const std::vector<std::string> &arguments)
{
    for (int i = 0; i < arguments.size(); i++) {

        // -- arguments format "key:value"
        int posSep = arguments.at(i).find(":", 0);
        if (posSep == std::string::npos) {
            std::cout << "Options must be formatted as follow : \"key:value\"" << std::endl;
            continue;
        }

        std::string key = arguments.at(i).substr(0, posSep);
        std::string value = arguments.at(i).substr(posSep+1);

        // -- name file with path of logging file
        if (key == "logPath") {
            pathLog = value;
        }

        // -- priority level of logging system
        else if (key == "priority") {
            if (value == "debug") {
                priority = Poco::Message::PRIO_DEBUG;
            }
            else if (value == "warning") {
                priority = Poco::Message::PRIO_WARNING;
            }
            else if (value == "error") {
                priority = Poco::Message::PRIO_ERROR;
            }
            else if (value == "information") {
                priority = Poco::Message::PRIO_INFORMATION;
            }
            else {
                std::cout << "Value " + value + "of option " + key + " not recognized ! Default warning is taken." << std::endl;
                priority = Poco::Message::PRIO_WARNING;
            }
        }
        else {
           std::cout << "Option " + key + " not recognized !" << std::endl;
        }
    }


    initLogger();
    Poco::Logger::get("ServiceCentral").information("Logger initialized !", __FILE__, __LINE__);

    if (!initContext()) {
        return EXIT_FAILURE;
    }
    Poco::Logger::get("ServiceCentral").information("Context loaded !", __FILE__, __LINE__);

    initThreads();
    Poco::Logger::get("ServiceCentral").information("Threads started !", __FILE__, __LINE__);

    waitForTerminationRequest();

    return EXIT_OK;
}

bool ServiceCentral::initContext()
{
    CentralContext *context = CentralContext::getCurrentContext();
    if (!context->initDatabase()) {
        Poco::Logger::get("ServiceCentral").error("LoadDataBase in error !", __FILE__, __LINE__);
        return false;
    }


    if (!context->startHttpCmdHandler()) {
        Poco::Logger::get("ServiceCentral").error("startHttpCmdHandler in error !", __FILE__, __LINE__);
        return false;
    }

    return true;
}

bool ServiceCentral::initThreads()
{
    contextThread = new ContextCentralThread();
    contextThread->startThread();
    
    releaseThread = new ReleasePrepareThread();
    //releaseThread->startThread();
    serversThread = new StateServerThread();
    //serversThread->startThread();
    syncThread = new StateServerThread();
    //syncThread->startThread();

    return true;
}