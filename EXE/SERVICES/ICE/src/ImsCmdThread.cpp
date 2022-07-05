#include "ImsCmdThread.h"

ImsCmdThread::ImsCmdThread()
{
    stop = false;
    thread = nullptr;
    readyToStart = false;
}
    
ImsCmdThread::~ImsCmdThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}

void ImsCmdThread::startThread()
{
    thread = new Poco::Thread("Ims");
    thread->setPriority(Poco::Thread::Priority::PRIO_NORMAL);
    thread->start(*this);
}

void ImsCmdThread::addCommand(std::shared_ptr<Command> cmd)
{
    while (!commandsMutex.tryLock()) {usleep(20);}
    commands.insert_or_assign(cmd->getUuid(), cmd);
    commandsMutex.unlock();
}

void ImsCmdThread::run() {

    ApplicationContext* context = ApplicationContext::getCurrentContext();
    ImsCommunicator* ims = context->getIms();
    ImsMessageHandler* handler = ims->getHandler();
    uint16_t waitTime = 300;
    Poco::Stopwatch watch;

    uint64_t counter = 0;

    while (!stop) {
	    
        watch.reset();
        watch.start();

        // -- refresh list of cplsInfos every 30 seconds
        if (ims->getHandler()->isLoggedIn()) {
            if (counter++ % 2000 == 0) fillCplsInfo();
            readyToStart = true;
        }

        while (!commandsMutex.tryLock()) {usleep(20);}
        int sizeCommands = commands.size();
        commandsMutex.unlock();

        int sleepTime = waitTime*1000-watch.elapsed();

        // -- execute command in time
        // -- treat nesting cmd if time
        while (sleepTime >= 1000 && sizeCommands > 0) {
            while (!commandsMutex.tryLock()) {usleep(20);}
            std::map<std::string, std::shared_ptr<Command>>::iterator it = commands.begin();
            commandsMutex.unlock();

            std::shared_ptr<Command> cmd = it->second;
            if (ims->getHandler()->isLoggedIn()) {
                executeCommand(cmd);
                while (!commandsMutex.tryLock()) {usleep(20);}
                commands.erase(it);
                sizeCommands = commands.size();
                commandsMutex.unlock();
            }
            else {
                usleep(10000);
            }

            sleepTime = waitTime*1000-watch.elapsed();
        }

        // -- wait for next send request
        if (sleepTime > 0) {
            usleep(sleepTime);
        }
        else {
            Poco::Logger::get("ImsCmdThread").warning("Thread exceeding time of " + std::to_string(-sleepTime) + "µs", __FILE__, __LINE__);
        }
    }
}

void ImsCmdThread::fillCplsInfo()
{
    ApplicationContext* context = ApplicationContext::getCurrentContext();
    ImsCommunicator* ims = context->getIms();
    ImsMessageHandler* handler = ims->getHandler();
    std::shared_ptr<ImsRequest> getCplsList = std::shared_ptr<ImsRequest>(handler->createGetListCplsRequest());
    std::shared_ptr<ImsResponse> response = std::make_shared<ImsResponse>(ImsResponse("", Command::GET_CPL_INFOS));
    std::map<std::string, std::shared_ptr<CplInfos>> listOfCpls;
    std::map<std::string, std::shared_ptr<CplInfos>> listOfCplsInfo;

    switch (ims->getProtocol()) {

        case ImsRequest::HTTP:
        // -- first request
        while (!ims->tryLock()) {usleep(20);}
        ims->sendRequest(getCplsList, response);
        listOfCpls = std::map<std::string, std::shared_ptr<CplInfos>>(response->getListOfCpls());
        response->clearMap();
        ims->unlock();

        // -- if ok then we ask for infos
        if (response->getStatus() == ImsResponse::OK) {
           
            std::map<std::string, std::shared_ptr<CplInfos>>::iterator it;
            for (it = listOfCpls.begin(); it != listOfCpls.end(); it++) {

                int pos = it->first.find_last_of(":");
                if (!context->hasCplId(it->first.substr(pos+1))) {

                    std::shared_ptr<ImsRequest> getCplsInfos = std::shared_ptr<ImsRequest>(handler->createGetCplInfoRequest(it->first));
                    std::shared_ptr<ImsResponse> responseInfo = std::make_shared<ImsResponse>(ImsResponse("", Command::GET_CPL_INFOS));

                    while (!ims->tryLock()) {usleep(20);}
                    ims->sendRequest(getCplsInfos, responseInfo);
                    listOfCplsInfo = std::map<std::string, std::shared_ptr<CplInfos>>(responseInfo->getListOfCpls());
                    ims->unlock();
                
                    if (responseInfo->getStatus() == ImsResponse::KO) {
                        Poco::Logger::get("ImsCmdThread").error("Error when retrieving info CPL ! ", __FILE__, __LINE__);
                    }
                    else {
                        std::map<std::string, std::shared_ptr<CplInfos>>::iterator itInfo;
                        for (itInfo = listOfCplsInfo.begin(); itInfo != listOfCplsInfo.end(); itInfo++) {
                            Poco::Logger::get("ImsCmdThread").information("Adding CPLinfo for cplId : " + itInfo->second->getCplId());
                            context->addCPLInfo(itInfo->second);
                        }
                    }
                }
            }
        }
        else {
            Poco::Logger::get("ImsCmdThread").error("Error when retrieving list of CPL ! ", __FILE__, __LINE__);
        }
        break;

        case ImsRequest::HTTPS:
        while (!ims->tryLock()) {usleep(20);}
        ims->sendRequest(getCplsList, response);
        ims->unlock();
        break;

        case ImsRequest::UDP:
        while (!ims->tryLock()) {usleep(20);}
        ims->sendRequest(getCplsList, response);
        ims->unlock();
        break;
    }
}

void ImsCmdThread::executeCommand(std::shared_ptr<Command> cmd)
{
    // -- create response to CMD
    ApplicationContext* context = ApplicationContext::getCurrentContext();
    ImsCommunicator* ims = context->getIms();
    ImsMessageHandler* handler = ims->getHandler();
    std::shared_ptr<ImsResponse> response = std::make_shared<ImsResponse>(ImsResponse(cmd->getUuid(), cmd->getType()));

    if (cmd->getType() == Command::GET_CPL_CONTENT) {
        std::string cplId = cmd->getStringParameter("cplId");
        if (cplId == "") {
            response->setStatus(CommandResponse::KO);
            response->setComments("Paramter cplId is not found !");
        }
        else {

            // -- send request to IMS : response is set by the handler when parsing response
            std::shared_ptr<ImsRequest> contentCpl(handler->createGetCplContentRequest(cplId));
            while (!ims->tryLock()) {usleep(20);}
            ims->sendRequest(contentCpl, response);
            ims->unlock();
        }
    }
    if (cmd->getType() == Command::GET_CPL_CONTENT_NAME) {

        std::string search = cmd->getStringParameter("search", "");
        if (search.empty()) {
            response->setStatus(CommandResponse::KO);
            response->setComments("Search string is empty !");
        }
        else {

            // -- copy map of known CPLS
            while (!context->tryLockCplsMutex()) {usleep(20);}
            std::map<std::string, std::shared_ptr<CplInfos>> mapCpl = context->getCplsInfos();
            context->unlockCplsMutex();

            std::map<std::string, std::shared_ptr<CplInfos>>::iterator it;
            std::shared_ptr<ImsResponse> responseTemp = std::make_shared<ImsResponse>(ImsResponse(cmd->getUuid(), cmd->getType()));

            int nbFoundCpls = 0;

            // -- parsing known CPLS to find title with search string in it
            for (it = mapCpl.begin(); it != mapCpl.end(); it++) {
                if (it->second->getCplTitle().find(search) != std::string::npos) {

                    // -- sending get content to IMS if match search string
                    std::shared_ptr<ImsRequest> contentCpl(handler->createGetCplContentRequest(it->second->getCplId()));
                    while (!ims->tryLock()) {usleep(20);}
                    ims->sendRequest(contentCpl, responseTemp);
                    ims->unlock();

                    // -- get content and fill final response of all contents with this one
                    if (responseTemp->getStatus() == CommandResponse::OK) {
                        std::shared_ptr<CplInfos> info = std::make_shared<CplInfos>(CplInfos());
                        info->setContentFile(responseTemp->getContentFile());
                        response->addNewCpl(it->second->getCplId(), info);
                        nbFoundCpls++;
                    }
                }
            }

            response->setStatus(CommandResponse::OK);
            response->setComments(std::to_string(nbFoundCpls) + " CPLS found for search string : " + search);
        }
    }
    context->getCommandHandler()->addResponse(response);
}