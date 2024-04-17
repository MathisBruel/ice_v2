#include "ReleasePrepareThread.h"

ReleasePrepareThread::ReleasePrepareThread()
{
    stop = false;
    thread = nullptr;
}
    
ReleasePrepareThread::~ReleasePrepareThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}

void ReleasePrepareThread::startThread()
{
    thread = new Poco::Thread("ReleasePrepareThread");
    thread->setPriority(Poco::Thread::Priority::PRIO_HIGH);
    thread->start(*this);
}

void ReleasePrepareThread::run()
{
    Poco::Stopwatch watch;
    CentralContext* context = CentralContext::getCurrentContext();

    // -- works at 5 min rate
    int waitTime = 300000;
    while (!stop) {
	    
        watch.reset();
        watch.start();

        std::map<int, std::shared_ptr<Release>> releases = context->prepareRelease();
        cinemas = context->prepareCinemas();

        // -- for all releases
        std::map<int, std::shared_ptr<Release>>::iterator itRelease;
        for (itRelease = releases.begin(); itRelease != releases.end(); itRelease++) {

            std::shared_ptr<Release> release = itRelease->second;
            Poco::Logger::get("ReleasePrepareThread").information("Treat release " + release->getName(), __FILE__, __LINE__);

            Poco::DateTime currentDate;
            int64_t timestampCurrent = currentDate.timestamp().epochMicroseconds();
            int64_t inDate = release->getInDate()->timestamp().epochMicroseconds();

            // -- valid if already available or available at least in the next 7 days
            if ((inDate - timestampCurrent) > 604800000) {
                continue;
            }

            // -- valid 24 hours after end validity
            std::shared_ptr<Poco::DateTime> outDatetime = release->getOutDate();
            if (outDatetime != nullptr) {
                int64_t outDate = outDatetime->timestamp().epochMicroseconds();
                if ((timestampCurrent - outDate) > 86400000) {
                    continue;
                }
            }

            Poco::Logger::get("ReleasePrepareThread").information("Check all associated cinemas !", __FILE__, __LINE__);
            idCinemas = context->associateRelatedCinema(release);
            Poco::Logger::get("ReleasePrepareThread").information("Check script present or not !", __FILE__, __LINE__);
            std::shared_ptr<ScriptState> scriptState = checkScriptAssociated(release);
            context->addScriptsInfos(release->getId(), scriptState);
            Poco::Logger::get("ReleasePrepareThread").information("Download all cpl found for all associated cinemas !", __FILE__, __LINE__);
            updateAssociatedCpl(release);

            std::map<int, std::shared_ptr<ScriptState>>::iterator itScriptRelease;
            std::map<int, std::shared_ptr<ScriptState>> scriptRelease(context->getScriptsInfos());
            for (itScriptRelease = scriptRelease.begin(); itScriptRelease != scriptRelease.end(); itScriptRelease++) {
                if (releases.find(itScriptRelease->first) == releases.end()) {
                    context->removeScriptsInfos(itScriptRelease->first);
                }
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

std::shared_ptr<ScriptState> ReleasePrepareThread::checkScriptAssociated(std::shared_ptr<Release> release)
{
    CentralContext* context = CentralContext::getCurrentContext();

    std::shared_ptr<ScriptState> state = std::make_shared<ScriptState>(ScriptState(release->getId()));
    std::map<int, std::shared_ptr<DownloadedScripts>> downloadedForAudi(context->getDownloadedScripts());

    std::shared_ptr<Script> featureScript = release->getFeatureScript();
    std::shared_ptr<Script> loopScript = release->getLoopScript();
    std::shared_ptr<Script> sasScript = release->getSasScript();

    for (int idCinema : idCinemas) {
        std::map<int, std::shared_ptr<Cinema>>::iterator it = cinemas.find(idCinema);
        if (it == cinemas.end()) {
            Poco::Logger::get("ReleasePrepareThread").warning("Cinema with id not found : " + std::to_string(idCinema), __FILE__, __LINE__);
            continue;
        }

        // -- foreach auditoriums
        for (std::shared_ptr<Auditorium> audiCinema : it->second->getAuditoriums()) {

            // -- search downloaded scripts for the auditorium
            std::map<int, std::shared_ptr<DownloadedScripts>>::iterator itDownload = downloadedForAudi.find(audiCinema->getId());
            if (itDownload == downloadedForAudi.end()) {
                Poco::Logger::get("ReleasePrepareThread").warning("Downloaded script with id of auditorium not found : " + std::to_string(audiCinema->getId()), __FILE__, __LINE__);
                continue;
            }

            if (featureScript != nullptr) {
                bool found = false;
                for (std::shared_ptr<ScriptInfoFile> info : itDownload->second->getScripts()) {
                    if (info->getNameScript() == featureScript->getName()) {
                        state->addScriptFeature(audiCinema->getId(), info);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    state->addScriptFeature(audiCinema->getId(), nullptr);
                }
            }
            if (loopScript != nullptr) {
                bool found = false;
                for (std::shared_ptr<ScriptInfoFile> info : itDownload->second->getScripts()) {
                    if (info->getNameScript() == loopScript->getName()) {
                        state->addScriptLoop(audiCinema->getId(), info);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    state->addScriptFeature(audiCinema->getId(), nullptr);
                }
            }
            if (sasScript != nullptr) {
                bool found = false;
                for (std::shared_ptr<ScriptInfoFile> info : itDownload->second->getScripts()) {
                    if (info->getNameScript() == sasScript->getName()) {
                        state->addScriptSas(audiCinema->getId(), info);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    state->addScriptFeature(audiCinema->getId(), nullptr);
                }
            }
        }
    }
}

void ReleasePrepareThread::updateAssociatedCpl(std::shared_ptr<Release> release)
{
    CentralContext* context = CentralContext::getCurrentContext();
    std::string pathRelease = "/data/ICE/Releases/" + release->getName() + "/Feature/";

    std::map<int, std::shared_ptr<Cinema>>::iterator itCinema;
    for (itCinema = cinemas.begin(); itCinema != cinemas.end(); itCinema++) {

        std::shared_ptr<Cinema> cinema = itCinema->second;
        for (std::shared_ptr<Auditorium> audi : cinema->getAuditoriums()) {

            // -- get first responding ICE server
            std::shared_ptr<Server> featureServer = audi->getFeatureServer();
            std::shared_ptr<Server> sasServer = audi->getSasServer();
            if (featureServer == nullptr && sasServer == nullptr) {
                Poco::Logger::get("ReleasePrepareThread").warning("No server added to auditorium : " + std::to_string(audi->getId()), __FILE__, __LINE__);
                break;
            }
            std::shared_ptr<Server> server = featureServer;
            if (featureServer == nullptr) {
                server = sasServer;
            }

            std::string sessionId = "";
            Poco::Net::HTTPClientSession* sessionICE = new Poco::Net::HTTPClientSession(server->getIp(), 8300);
            sessionICE->setTimeout(Poco::Timespan(10, 0));

            while (!context->tryLockSessionICEMutex()) {Timer::crossUsleep(1000);}
            sessionId = loginServerICE(sessionICE);

            if (sessionId.empty()) {
                Poco::Logger::get("ReleasePrepareThread").warning("No server connection done on : " + std::to_string(server->getId()), __FILE__, __LINE__);
                break;
            }

            std::map<int, std::shared_ptr<PresentCpl>> cpls(context->getPresentCpls());
            std::map<int, std::shared_ptr<PresentCpl>>::iterator itCplAudi = cpls.find(audi->getId());

            if (itCplAudi == cpls.end()) {
                Poco::Logger::get("ReleasePrepareThread").warning("No cpls backup from auditorium : " + std::to_string(audi->getId()), __FILE__, __LINE__);
                break;
            }

            
            for (std::shared_ptr<Cpl> cpl : itCplAudi->second->getCpls()) {
                std::string titleCpl = cpl->getName();

                // -- must start with global pattern
                std::regex regexGlobal(release->getGlobalPattern());
                if (!std::regex_match(titleCpl, regexGlobal)) {
                    continue;
                }

                // -- if specific pattern given : must be found in title
                std::regex regexSpecific(release->getSpecificPattern());
                if (!release->getSpecificPattern().empty() && !std::regex_match(titleCpl, regexSpecific)) {
                    continue;
                }

                // -- check if file already exists
                std::string pathCpl = pathRelease + cpl->getUuid() + ".xml";
                Poco::File testPathCpl(pathCpl);
                if (testPathCpl.exists()) {
                    continue;
                }

                // -- get content of cpl and write it
                std::string content = getCplContent(sessionICE, sessionId, cpl->getUuid());
                std::ofstream outCplFile(pathCpl, std::ios::out);
                outCplFile << content;
                outCplFile.close();

                // -- modify cpl and add link it to release
                Cpl cplSql;
                cplSql.setDatas(cpl->getUuid(), cpl->getName());
                cplSql.setCplInfos(pathCpl, "", Cpl::CplType::UnknownType);
                Query* query = cplSql.createQuery();
                ResultQuery* result = context->executeQuery(query);

                if (!result->isValid()) {
                    Poco::Logger::get("ReleasePrepareThread").warning("Insert cpl for release failed !", __FILE__, __LINE__);
                    delete query;
                    delete result;
                    break;
                }

                LinkParam link(LinkParam::CPL_RELEASE);
                link.setDatas(result->getLastInsertedId(), release->getId());
                Query* queryLink = link.createQuery();
                ResultQuery* resultLink = context->executeQuery(queryLink);

                if (!resultLink->isValid()) {
                    Poco::Logger::get("ReleasePrepareThread").warning("Insert cpl link for release failed !", __FILE__, __LINE__);
                }

                delete query;
                delete result;
                delete queryLink;
                delete resultLink;
            }

            if (!sessionId.empty()) {
                logoutICE(sessionICE, sessionId);
            }
            delete sessionICE;
            context->unlockSessionICEMutex();
        }
    }
}

std::string ReleasePrepareThread::loginServerICE(Poco::Net::HTTPClientSession* sessionICE)
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
    }

    return sessionId;
}

std::string ReleasePrepareThread::getCplContent(Poco::Net::HTTPClientSession* sessionICE, std::string sessionId, std::string uuid)
{
    Poco::Net::HTTPRequest httpRequest("POST", "/");
    httpRequest.setContentType("application/xml");

    std::string body = "<command><type>GET_CPL_CONTENT</type><parameters><cplId>" + uuid + "</cplId></parameters></command>";
    Poco::Net::NameValueCollection cookies;
    cookies.add("sessionId", sessionId);
    httpRequest.setCookies(cookies);

    try {
        // -- send request
        sessionICE->sendRequest(httpRequest) << body << std::flush;
    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("No possible to send http request", __FILE__, __LINE__);
        return "";
    }

    Poco::Net::HTTPResponse responseHttp;
    std::stringstream stringStream;
    try {
        std::istream& stream = sessionICE->receiveResponse(responseHttp);
        Poco::StreamCopier::copyStream(stream, stringStream);

    } catch (std::exception &e) {
        Poco::Logger::get("StateServerThread").error("No response received", __FILE__, __LINE__);
        return "";
    }

    // -- error state
    if (responseHttp.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
        return "";
    }

    // -- parse response
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    
    std::string content = stringStream.str();
    content = content.substr(0, content.size()-10);
    content = content.substr(9);

    return content;
}

void ReleasePrepareThread::logoutICE(Poco::Net::HTTPClientSession* sessionICE, std::string sessionId)
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