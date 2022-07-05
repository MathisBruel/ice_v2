#include "StateServerThread.h"

StateServerThread::StateServerThread()
{
    stop = false;
    thread = nullptr;
}
    
StateServerThread::~StateServerThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}
    
void StateServerThread::startThread()
{
    thread = new Poco::Thread("StateServerThread");
    thread->setPriority(Poco::Thread::Priority::PRIO_HIGH);
    thread->start(*this);
    Poco::Logger::get("StateServerThread").debug("Thread id : " + std::to_string(thread->currentTid()), __FILE__, __LINE__);
}

void StateServerThread::run()
{
    Poco::Stopwatch watch;
    CentralContext* context = CentralContext::getCurrentContext();

    // -- works at 5 min rate
    int waitTime = 300000;
    while (!stop) {
	    
        watch.reset();
        watch.start();

        std::map<int, std::shared_ptr<Auditorium>> auditoriums = context->prepareAuditoriums();
        std::map<int, std::shared_ptr<Server>> servers = context->prepareServers();

        // -- foreach auditprium
        std::map<int, std::shared_ptr<Auditorium>>::iterator itAudi;
        for (itAudi = auditoriums.begin(); itAudi != auditoriums.end(); itAudi++) {
            std::shared_ptr<Auditorium> audi = itAudi->second;

            Poco::Logger::get("StateServerThread").information("Auditorium " + audi->getName() + " test !", __FILE__, __LINE__);

            std::shared_ptr<StateServer> stateAuditorium = checkResponseIms(audi);
            context->addStateAuditorium(stateAuditorium);

            std::shared_ptr<Server> featureServer = audi->getFeatureServer();
            if (featureServer == nullptr) {
                Poco::Logger::get("StateServerThread").warning("Auditorium " + audi->getName() + " has no feature server configured !", __FILE__, __LINE__);
                continue;
            }

            Poco::Logger::get("StateServerThread").information("Server feature test !", __FILE__, __LINE__);
            std::shared_ptr<StateServer> stateServerFeature = checkServer(featureServer, true);
            context->addStateServer(stateServerFeature);

            std::shared_ptr<Server> sasServer = audi->getSasServer();
            if (sasServer == nullptr) {
                Poco::Logger::get("StateServerThread").warning("Auditorium " + audi->getName() + " has no sas server configured !", __FILE__, __LINE__);
                continue;
            }

            Poco::Logger::get("StateServerThread").information("Server sas test !", __FILE__, __LINE__);
            std::shared_ptr<StateServer> stateServerSas = checkServer(sasServer);
            context->addStateServer(stateServerSas);
        }

        // -- delete no more referenced auditoriums
        std::map<int, std::shared_ptr<StateServer>>::iterator itStateAudi;
        std::map<int, std::shared_ptr<StateServer>> stateAudi(context->getStateAuditoriums());
        for (itStateAudi = stateAudi.begin(); itStateAudi != stateAudi.end(); itStateAudi++) {
            if (auditoriums.find(itStateAudi->first) == auditoriums.end()) {
                context->removeStateAuditorium(itStateAudi->first);
            }
        }
        std::map<int, std::shared_ptr<DownloadedScripts>>::iterator itDownloadScript;
        std::map<int, std::shared_ptr<DownloadedScripts>> downloaded(context->getDownloadedScripts());
        for (itDownloadScript = downloaded.begin(); itDownloadScript != downloaded.end(); itDownloadScript++) {
            if (auditoriums.find(itDownloadScript->first) == auditoriums.end()) {
                context->removeDownloadedScripts(itDownloadScript->first);
            }
        }
        std::map<int, std::shared_ptr<PresentCpl>>::iterator itPresentCpl;
        std::map<int, std::shared_ptr<PresentCpl>> presents(context->getPresentCpls());
        for (itPresentCpl = presents.begin(); itPresentCpl != presents.end(); itPresentCpl++) {
            if (auditoriums.find(itPresentCpl->first) == auditoriums.end()) {
                context->removePresentCpls(itPresentCpl->first);
            }
        }

        // -- delete no more referenced servers
        std::map<int, std::shared_ptr<StateServer>>::iterator itStateServer;
        std::map<int, std::shared_ptr<StateServer>> stateServer(context->getStateServers());
        for (itStateServer = stateServer.begin(); itStateServer != stateServer.end(); itStateServer++) {
            if (servers.find(itStateServer->first) == servers.end()) {
                context->removeStateServer(itStateServer->first);
            }
        }

        int sleepTime = waitTime*1000-watch.elapsed();

        // -- wait for next send request
        if (sleepTime > 0) {
            usleep(sleepTime);
        }
        else {
            Poco::Logger::get("StateServerThread").warning("Thread exceeding time of " + std::to_string(-sleepTime) + "µs", __FILE__, __LINE__);
        }
    }
}

std::shared_ptr<StateServer> StateServerThread::checkResponseIms(std::shared_ptr<Auditorium> auditorium)
{
    std::shared_ptr<StateServer> state = std::make_shared<StateServer>(auditorium->getId(), true);

    // -- check Ims response
    ImsMessageHandler* handler = nullptr;
    ImsRequest::Protocol protocol = ImsRequest::HTTP;
    if (auditorium->getTypeIms() == Auditorium::BARCO) {
        handler = new BarcoHandler();
        protocol = ImsRequest::HTTPS;
    }
    else if (auditorium->getTypeIms() == Auditorium::CHRISTIE) {
        handler = new ChristieHandler();
        protocol = ImsRequest::UDP;
    }
    else if (auditorium->getTypeIms() == Auditorium::DOLBY) {
        handler = new DoremiHandler();
    }
    ImsCommunicator* communicatorIms = new ImsCommunicator(auditorium->getIp(), auditorium->getPort(), protocol);
    communicatorIms->addMessageHandler(handler);
    communicatorIms->open();

    // -- send login
    std::shared_ptr<ImsRequest> requestLogin = std::shared_ptr<ImsRequest>(handler->createLoginRequest(auditorium->getUser(), auditorium->getPass()));
    std::shared_ptr<ImsResponse> response = std::make_shared<ImsResponse>(ImsResponse("", Command::UNKNOW_COMMAND));
    while (!communicatorIms->tryLock()) {Timer::crossUsleep(25);}
    communicatorIms->sendRequest(requestLogin, response);
    communicatorIms->unlock();

    if (response->getStatus() == CommandResponse::OK) {state->setAnswering(true);}
    else {state->setAnswering(false, response->getComments());}

    // -- send logout
    std::shared_ptr<ImsRequest> requestLogout = std::shared_ptr<ImsRequest>(handler->createLogoutRequest());
    while (!communicatorIms->tryLock()) {Timer::crossUsleep(25);}
    communicatorIms->sendRequest(requestLogout, response);
    communicatorIms->unlock();

    // -- close and free resources
    communicatorIms->close();
    delete communicatorIms;
    delete handler;

    return state;
}
std::shared_ptr<StateServer> StateServerThread::checkServer(std::shared_ptr<Server> server, bool checkListCpls)
{
    CentralContext* context = CentralContext::getCurrentContext();
    std::shared_ptr<StateServer> state = std::make_shared<StateServer>(StateServer(server->getId(), false));

    // -- open session to ICE server
    std::string sessionId = "";
    Poco::Net::HTTPClientSession* sessionICE = new Poco::Net::HTTPClientSession(server->getIp(), 8300);
    sessionICE->setTimeout(Poco::Timespan(10, 0));

    Poco::Logger::get("StateServerThread").information("Server session test !", __FILE__, __LINE__);
    while (!context->tryLockSessionICEMutex()) {Timer::crossUsleep(1000);}
    sessionId = loginServerICE(sessionICE);

    if (!sessionId.empty()) {

        state->setAnswering(true);

        Poco::Logger::get("StateServerThread").information("Server devices test !", __FILE__, __LINE__);
        std::vector<std::string> devicesInError = checkDeviceICE(sessionICE, sessionId);
        for (std::string device : devicesInError) {
            state->addDeviceInError(device);
        }

        Poco::Logger::get("StateServerThread").information("Server get scripts test !", __FILE__, __LINE__);
        std::vector<std::shared_ptr<ScriptInfoFile>> scripts = getScriptsICE(sessionICE, sessionId);
        std::shared_ptr<DownloadedScripts> downloaded = std::make_shared<DownloadedScripts>(DownloadedScripts(server->getIdAuditorium()));
        for (std::shared_ptr<ScriptInfoFile> script : scripts) {
            downloaded->addScript(script);
        }
        context->addDownloadedScripts(downloaded);

        if (checkListCpls) {
            Poco::Logger::get("StateServerThread").information("Server get cpl test !", __FILE__, __LINE__);
            std::vector<std::shared_ptr<Cpl>> cpls = getListCpls(sessionICE, sessionId);
            std::shared_ptr<PresentCpl> presentCpl = std::make_shared<PresentCpl>(PresentCpl(server->getIdAuditorium()));
            for (std::shared_ptr<Cpl> cpl : cpls) {
                presentCpl->addCpl(cpl);
            }
            context->addPresentCpls(presentCpl);
        }
        logoutICE(sessionICE, sessionId);
    }
    else {
        state->setAnswering(false);
    }

   delete sessionICE;
   context->unlockSessionICEMutex();
   return state;
}

std::string StateServerThread::loginServerICE(Poco::Net::HTTPClientSession* sessionICE)
{
    std::string sessionId = "";
    Poco::Net::HTTPRequest httpRequest("POST", "/login");
    httpRequest.setContentType("application/xml");
    std::string body = "<command><username>admin</username><password>ice17180</password></command>";
    httpRequest.setContentLength(body.length());
    try {
        // -- send request and set timestamp
        sessionICE->sendRequest(httpRequest) << body << std::flush;
    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("No possible to send http request", __FILE__, __LINE__);
        return "";
    }

    Poco::Net::HTTPResponse responseHttp;
    try {
        std::istream& stream = sessionICE->receiveResponse(responseHttp);
        std::vector<Poco::Net::HTTPCookie> cookies;
        responseHttp.getCookies(cookies);
        std::vector<Poco::Net::HTTPCookie>::iterator cookieIt = cookies.begin();
        for (; cookieIt != cookies.end(); cookieIt++) {

            if (cookieIt->getName() == "sessionId") {
                sessionId = cookieIt->getValue();
            }
        }
    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("No response received", __FILE__, __LINE__);
        return sessionId;
    }

    return sessionId;
}
void StateServerThread::logoutICE(Poco::Net::HTTPClientSession* sessionICE, std::string sessionId)
{
    Poco::Net::HTTPRequest httpRequest("POST", "/logout");
    httpRequest.setContentType("application/xml");
    std::string body = "<command><username>admin</username><password>ice17180</password></command>";
    httpRequest.setContentLength(body.length());

    Poco::Net::NameValueCollection cookies;
    cookies.add("sessionId", sessionId);
    httpRequest.setCookies(cookies);

    try {
        // -- send request and set timestamp
        sessionICE->sendRequest(httpRequest) << body << std::flush;
    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("No possible to send http request", __FILE__, __LINE__);
    }
}

std::vector<std::string> StateServerThread::checkDeviceICE(Poco::Net::HTTPClientSession* sessionICE, std::string sessionId)
{
    CentralContext* context = CentralContext::getCurrentContext();
    Poco::Net::HTTPRequest httpRequest("POST", "/");
    httpRequest.setContentType("application/xml");

    std::string body = "<command><type>LIST_DEVICES</type><parameters></parameters></command>";

    Poco::Net::NameValueCollection cookies;
    cookies.add("sessionId", sessionId);
    httpRequest.setCookies(cookies);

    try {
        // -- send request
        sessionICE->sendRequest(httpRequest) << body << std::flush;
    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("No possible to send http request", __FILE__, __LINE__);
        return std::vector<std::string>();
    }

    Poco::Net::HTTPResponse responseHttp;
    std::stringstream stringStream;
    try {
        std::istream& stream = sessionICE->receiveResponse(responseHttp);
        Poco::StreamCopier::copyStream(stream, stringStream);

    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("No response received", __FILE__, __LINE__);
        return std::vector<std::string>();
    }

    // -- error state
    if (responseHttp.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
        return std::vector<std::string>();
    }

    // -- parse response
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    
    try {
        doc = parser.parseString(stringStream.str());
    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("Exception when parsing XML file !", __FILE__, __LINE__);
        Poco::Logger::get("StateServerThread").debug("Content : " + stringStream.str(), __FILE__, __LINE__);
        return std::vector<std::string>();
    }

    // -- check active device
    Poco::XML::NodeList* devices = doc->getElementsByTagName("Device");
    std::vector<std::string> devicesFinal;

    for (int i = 0; i < devices->length(); i++) {
        std::string name;

        Poco::XML::NodeList* devicesAttributes = devices->item(i)->childNodes();
        for (int j = 0; j < devicesAttributes->length(); j++) {
            if (devicesAttributes->item(j)->nodeName() == "Name") {
                name = devicesAttributes->item(j)->innerText();
            }
            else if (devicesAttributes->item(j)->nodeName() == "Active") {
                std::string active = devicesAttributes->item(j)->innerText();
                if (active != "true") {
                    devicesFinal.push_back(name);
                }
            }
        }
        devicesAttributes->release();
    }

    // -- set final values
    devices->release();
    doc->release();
    return devicesFinal;
}
std::vector<std::shared_ptr<ScriptInfoFile>> StateServerThread::getScriptsICE(Poco::Net::HTTPClientSession* sessionICE, std::string sessionId)
{
    CentralContext* context = CentralContext::getCurrentContext();
    Poco::Net::HTTPRequest httpRequest("POST", "/");
    httpRequest.setContentType("application/xml");

    std::string body = "<command><type>GET_DOWNLOADED_SCRIPTS</type><parameters></parameters></command>";

    Poco::Net::NameValueCollection cookies;
    cookies.add("sessionId", sessionId);
    httpRequest.setCookies(cookies);

    try {
        // -- send request
        sessionICE->sendRequest(httpRequest) << body << std::flush;
    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("No possible to send http request", __FILE__, __LINE__);
        return std::vector<std::shared_ptr<ScriptInfoFile>>();
    }

    Poco::Net::HTTPResponse responseHttp;
    std::stringstream stringStream;
    try {
        std::istream& stream = sessionICE->receiveResponse(responseHttp);
        Poco::StreamCopier::copyStream(stream, stringStream);

    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("No response received", __FILE__, __LINE__);
        return std::vector<std::shared_ptr<ScriptInfoFile>>();
    }

    // -- error state
    if (responseHttp.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
        return std::vector<std::shared_ptr<ScriptInfoFile>>();
    }

    // -- parse response
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    
    try {
        doc = parser.parseString(stringStream.str());
    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("Exception when parsing XML file !", __FILE__, __LINE__);
        Poco::Logger::get("StateServerThread").debug("Content : " + stringStream.str(), __FILE__, __LINE__);
        return std::vector<std::shared_ptr<ScriptInfoFile>>();
    }

    // -- parse script list
    std::vector<std::shared_ptr<ScriptInfoFile>> toReturn;
    Poco::XML::NodeList* scriptsList = doc->getElementsByTagName("Script");
    for (int i = 0; i < scriptsList->length(); i++) {

        std::shared_ptr<ScriptInfoFile> infoScript = std::make_shared<ScriptInfoFile>(ScriptInfoFile());

        Poco::XML::NodeList* scriptAttributes = scriptsList->item(i)->childNodes();
        for (int j = 0; j < scriptAttributes->length(); j++) {

            if (scriptAttributes->item(j)->nodeName() == "Name") {
                infoScript->setNameScript(scriptAttributes->item(j)->innerText());
            }
            else if (scriptAttributes->item(j)->nodeName() == "cis") {
                Poco::XML::NamedNodeMap* attrib = scriptAttributes->item(j)->attributes();
                std::string percent = attrib->getNamedItem("percent")->innerText();
                infoScript->setCisInfo(scriptAttributes->item(j)->innerText(), std::stoi(percent));
                attrib->release();
            }
            else if (scriptAttributes->item(j)->nodeName() == "Lvi") {
                infoScript->setLviInfo(scriptAttributes->item(j)->innerText(), 100);
            }
            else if (scriptAttributes->item(j)->nodeName() == "Keep") {
                infoScript->setPriority(scriptAttributes->item(j)->innerText() == "true");
            }
            else if (scriptAttributes->item(j)->nodeName() == "Played") {
                if (scriptAttributes->item(j)->innerText() == "true") infoScript->setAlreadyPlayed();
            }
            else if (scriptAttributes->item(j)->nodeName() == "Syncs") {

                Poco::XML::NodeList* syncs = scriptAttributes->item(j)->childNodes();
                for (int k = 0; k < syncs->length(); k++) {
                    
                    if (syncs->item(k)->nodeName() == "Sync") {

                        std::string uuidCpl = syncs->item(k)->innerText();
                        uuidCpl = uuidCpl.substr(0, uuidCpl.length()-5);
                        int posName = uuidCpl.find_last_of("/");
                        if (posName == std::string::npos) {
                            continue;
                        }
                        uuidCpl = uuidCpl.substr(posName);

                        Poco::XML::NamedNodeMap* attributes = syncs->item(k)->attributes();
                        Poco::XML::Node* presentNode = attributes->getNamedItem("Present");
                        std::string presentStr = presentNode->innerText();
                        attributes->release();
                        infoScript->addSync(uuidCpl, "", presentStr == "true" ? 100 : 0);
                    }
                }
                syncs->release();
            }
        }
        scriptAttributes->release();
        toReturn.push_back(infoScript);
    }

    scriptsList->release();
    doc->release();
    return toReturn;
}
std::vector<std::shared_ptr<Cpl>> StateServerThread::getListCpls(Poco::Net::HTTPClientSession* sessionICE, std::string sessionId)
{
    CentralContext* context = CentralContext::getCurrentContext();
    Poco::Net::HTTPRequest httpRequest("POST", "/");
    httpRequest.setContentType("application/xml");

    std::string body = "<command><type>GET_CPL_INFOS</type><parameters></parameters></command>";

    Poco::Net::NameValueCollection cookies;
    cookies.add("sessionId", sessionId);
    httpRequest.setCookies(cookies);

    try {
        // -- send request
        sessionICE->sendRequest(httpRequest) << body << std::flush;
    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("No possible to send http request", __FILE__, __LINE__);
        return std::vector<std::shared_ptr<Cpl>>();
    }

    Poco::Net::HTTPResponse responseHttp;
    std::stringstream stringStream;
    try {
        std::istream& stream = sessionICE->receiveResponse(responseHttp);
        Poco::StreamCopier::copyStream(stream, stringStream);

    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("No response received", __FILE__, __LINE__);
        return std::vector<std::shared_ptr<Cpl>>();
    }

    // -- error state
    if (responseHttp.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
        return std::vector<std::shared_ptr<Cpl>>();
    }

    // -- parse response
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    
    try {
        doc = parser.parseString(stringStream.str());
    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("Exception when parsing XML file !", __FILE__, __LINE__);
        Poco::Logger::get("StateServerThread").debug("Content : " + stringStream.str(), __FILE__, __LINE__);
        return std::vector<std::shared_ptr<Cpl>>();
    }

    std::vector<std::shared_ptr<Cpl>> toReturn;
    Poco::XML::NodeList* cplsXml = doc->getElementsByTagName("CPL");
    for (int i = 0; i < cplsXml->length(); i++) {

        std::string uuid;
        std::string name;

        Poco::XML::NamedNodeMap* attributes = cplsXml->item(i)->attributes();
        for (int j = 0; j < attributes->length(); j++) {
            if (attributes->item(j)->nodeName() == "id") {
                uuid = attributes->item(j)->innerText();
            }
        }
        attributes->release();

        Poco::XML::NodeList* cplChilds = cplsXml->item(i)->childNodes();
        for (int j = 0; j < cplChilds->length(); j++) {
            if (cplChilds->item(j)->nodeName() == "title") {
                name = cplChilds->item(j)->innerText();
            }
        }
        cplChilds->release();

        std::shared_ptr<Cpl> cpl = std::make_shared<Cpl>(Cpl());
        cpl->setDatas(uuid, name);
        toReturn.push_back(cpl);
    }
    cplsXml->release();
    return toReturn;
}