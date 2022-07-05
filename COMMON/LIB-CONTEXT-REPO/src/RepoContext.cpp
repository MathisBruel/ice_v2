#include "RepoContext.h"

RepoContext* RepoContext::context = 0;

RepoContext* RepoContext::getCurrentContext()
{
    if (context == nullptr) {
        context = new RepoContext();
    }
    return context;
}

RepoContext::RepoContext() {}
RepoContext::~RepoContext() {}

bool RepoContext::startHttpCmdHandler()
{
    try {
        HttpRepoHandlerFactory* commandHandlerFactory = new HttpRepoHandlerFactory();
        Poco::Net::HTTPServerParams* params = new Poco::Net::HTTPServerParams();
        params->setTimeout(Poco::Timespan(7200, 0));
        params->setMaxKeepAliveRequests(200);
        Poco::Net::HTTPServer* server = new Poco::Net::HTTPServer(commandHandlerFactory, 8700, params);
        server->start();

    } catch (std::exception &e) {
        Poco::Logger::get("RepoContext").error("Error at initialization of http command receiver !", __FILE__, __LINE__);
        return false;
    }

    return true;
}