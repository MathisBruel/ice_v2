#include "ServiceICE.h"

ServiceICE::ServiceICE() 
{
    pathConfig = "";
    pathLog = "";
}
    
ServiceICE::~ServiceICE()
{
    Poco::Util::Application::uninitialize();
}

void ServiceICE::initialize(Application& self) {
    Application::initialize(self);
}
void ServiceICE::reinitialize(Application& self) {
    Application::reinitialize(self);
}
void ServiceICE::uninitialize() {}

void ServiceICE::initLogger()
{
    // -- change every 50 MB and save old with timestamp and compress it 
    // -- with a maximum of 5 archived files
    if (pathLog == "") {
        pathLog = "/data/ICE2/LOG/ICE-SERVER.log";
        std::cout << "PathLog not setted ! Default " + pathLog + " is taken." << std::endl;
    }
    Poco::Channel* pChannel = new Poco::FileChannel();
    pChannel->setProperty("path", pathLog);
    pChannel->setProperty("rotation", "100 M");
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

bool ServiceICE::initLibs()
{
    // -- initialize context
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    if (!context->initConfiguration(pathConfig)) {
        Poco::Logger::get("ServiceICE").error("Error at initialisation of configuration !", __FILE__, __LINE__);
        return false;
    }
    Poco::Logger::get("ServiceICE").information("Configuration initialized !", __FILE__, __LINE__);

    if (!context->initIms()) {
        Poco::Logger::get("ServiceICE").error("Error at initialisation of IMS !", __FILE__, __LINE__);
        return false;
    }
    Poco::Logger::get("ServiceICE").information("IMS initialized !", __FILE__, __LINE__);

    if (!context->initKinet()) {
        Poco::Logger::get("ServiceICE").error("Error at initialisation of KINET !", __FILE__, __LINE__);
        return false;
    }
    Poco::Logger::get("ServiceICE").information("KINET initialized !", __FILE__, __LINE__);

    if (!context->initRenderer()) {
        Poco::Logger::get("ServiceICE").error("Error at initialisation of RENDERER !", __FILE__, __LINE__);
        return false;
    }
    Poco::Logger::get("ServiceICE").information("RENDERER initialized !", __FILE__, __LINE__);

    if (!context->initRepos()) {
        Poco::Logger::get("ServiceICE").error("Error at initialisation of REPOS !", __FILE__, __LINE__);
        return false;
    }
    Poco::Logger::get("ServiceICE").information("REPOSITORY initialized !", __FILE__, __LINE__);

    if (!context->initHttpCommand()) {
        Poco::Logger::get("ServiceICE").error("Error at initialisation of Http command receiver !", __FILE__, __LINE__);
        return false;
    }
    Poco::Logger::get("ServiceICE").information("HTTP command receiver initialized !", __FILE__, __LINE__);

    if (!context->initUdpCommand()) {
        Poco::Logger::get("ServiceICE").error("Error at initialisation of UDP command receiver !", __FILE__, __LINE__);
        return false;
    }
    Poco::Logger::get("ServiceICE").information("UDP command receiver initialized !", __FILE__, __LINE__);

    if (!context->initStreamReceiver()) {
        Poco::Logger::get("ServiceICE").error("Error at initialisation of Stream receiver !", __FILE__, __LINE__);
        return false;
    }
    Poco::Logger::get("ServiceICE").information("Stream receiver initialized !", __FILE__, __LINE__);

    return true;
}

bool ServiceICE::initThreads()
{
    // -- IMS  
    imsThread = new ImsThread();
    imsThread->startThread();
    imsCmdThread = new ImsCmdThread();
    imsCmdThread->startThread();

    // -- KINET
    kinetThread = new KinetThread();
    kinetThread->startThread();
    discoveryThread = new KinetDiscoveryThread();
    discoveryThread->startThread();

    // -- REPO
    repoThread = new RepositoryThread();
    repoThread->startThread();
    repoCmdThread = new RepositoryCommandThread();
    repoCmdThread->startThread();
    downloadThread = new DownloadThread();
    downloadThread->startThread();

    // -- CONTEXT
    contextThread = new ContextThread();
    contextThread->startThread();

    // -- RENDERER
    renderThread = new RendererThread();
    renderThread->startThread();

    // -- COMMAND
    dispatcherCmdThread = new CommandDispatcherThread(imsCmdThread, kinetThread, repoCmdThread, contextThread, renderThread);
    dispatcherCmdThread->startThread();

    return true;
}

int ServiceICE::main(const std::vector<std::string> &arguments)
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

        // -- path to config files
        if (key == "configPath") {
            pathConfig = value;
        }

        // -- name file with path of logging file
        else if (key == "logPath") {
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
    Poco::Logger::get("ServiceICE").information("Logger initialized !", __FILE__, __LINE__);

    if (!initLibs()) {
        Poco::Logger::get("ServiceICE").error("Initialization of LIBS in error !", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }

    if (!initThreads()) {
        Poco::Logger::get("ServiceICE").error("Initialization of threads in error !", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }
    Poco::Logger::get("ServiceICE").information("Threads initialized !", __FILE__, __LINE__);

    waitForTerminationRequest();

    Poco::Logger::get("ServiceICE").information("Terminate ICE Service !", __FILE__, __LINE__);

    stopService();

    return EXIT_OK;
}

void ServiceICE::stopService()
{
    dispatcherCmdThread->stopThread();
    Poco::Logger::get("ServiceICE").information("DispatcherCmdThread Stopped !", __FILE__, __LINE__);

    renderThread->stopThread();
    Poco::Logger::get("ServiceICE").information("RenderThread Stopped !", __FILE__, __LINE__);

    contextThread->stopThread();
    Poco::Logger::get("ServiceICE").information("ContextThread Stopped !", __FILE__, __LINE__);

    downloadThread->stopThread();
    Poco::Logger::get("ServiceICE").information("DownloadThread Stopped !", __FILE__, __LINE__);

    repoCmdThread->stopThread();
    Poco::Logger::get("ServiceICE").information("RepoCmdThread Stopped !", __FILE__, __LINE__);

    repoThread->stopThread();
    Poco::Logger::get("ServiceICE").information("RepoThread Stopped !", __FILE__, __LINE__);

    discoveryThread->stopThread();
    Poco::Logger::get("ServiceICE").information("DiscoveryThread Stopped !", __FILE__, __LINE__);

    imsCmdThread->stopThread();
    Poco::Logger::get("ServiceICE").information("ImsCmdThread Stopped !", __FILE__, __LINE__);

    imsThread->stopThread();
    Poco::Logger::get("ServiceICE").information("ImsThread Stopped !", __FILE__, __LINE__);

    kinetThread->stopThread();
    Poco::Logger::get("ServiceICE").information("KinetThread Stopped !", __FILE__, __LINE__);

    // -- deinitialize context
    ApplicationContext* context = ApplicationContext::getCurrentContext();
    context->stopComponents();
    Poco::Logger::get("ServiceICE").information("Context Components Stopped !", __FILE__, __LINE__);
}