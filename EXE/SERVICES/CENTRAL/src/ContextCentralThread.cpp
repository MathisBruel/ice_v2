#include "ContextCentralThread.h"

#include "CentralContext.h"
#include "ContentOpsBoundary/BoundaryManager.h"

ContextCentralThread::ContextCentralThread() : _boundaryManager(BoundaryManager::GetInstance())
{
    stop = false;
    thread = nullptr;
}
    
ContextCentralThread::~ContextCentralThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}

void ContextCentralThread::startThread()
{
    thread = new Poco::Thread("ContextCentralThread");
    thread->setPriority(Poco::Thread::Priority::PRIO_HIGH);
    thread->start(*this);
    Poco::Logger::get("ContextCentralThread").debug("Thread id : " + std::to_string(thread->currentTid()), __FILE__, __LINE__);
}

void ContextCentralThread::run() {

    Poco::Stopwatch watch;
    CentralContext* context = CentralContext::getCurrentContext();
    _cobConfigurator = std::make_unique<COB_Configurator>();
    this->_dbConnection = _cobConfigurator->GetDBConnection().get();

    // -- works at 50 ms rate
    int waitTime = 50;
    while (!stop) {
	    
        watch.reset();
        watch.start();

        // -- try acquire mutex on commands & responses
        // -- wait for next send request
        std::shared_ptr<CommandCentral> cmd(context->getCommandHandler()->getFirstCommand());

        int sleepTime = waitTime*1000-watch.elapsed();
        while (cmd != nullptr && sleepTime >= 1000) {
                
            executeCommand(cmd);

            // -- we remove command from map
            context->getCommandHandler()->deleteCommand(cmd->getUuid());
            cmd = std::shared_ptr(context->getCommandHandler()->getFirstCommand());

            sleepTime = waitTime*1000-watch.elapsed();
        }

        // -- erase no more used response
        context->getCommandHandler()->removeOldResponses();
        sleepTime = waitTime*1000-watch.elapsed();

        // -- wait for next send request
        if (sleepTime > 0) {
            usleep(sleepTime);
        }
        else {
            Poco::Logger::get("ContextThread").warning("Thread exceeding time of " + std::to_string(-sleepTime) + "µs", __FILE__, __LINE__);
        }
    }
}

void ContextCentralThread::executeCommand(std::shared_ptr<CommandCentral> cmd)
{
    CentralContext* context = CentralContext::getCurrentContext();
    Poco::Logger::get("ContextCentralThread").debug("Execute cmd type : " + std::to_string(cmd->getType()));
    std::shared_ptr<CommandCentralResponse> response = std::make_shared<CommandCentralResponse>(CommandCentralResponse(cmd->getUuid(), cmd->getType()));
    // -- GETTERS
    if (cmd->getType() == CommandCentral::GET_GROUPS) {
        
        int id = cmd->getIntParameter("id");

        if (id == -1) {
            Query* query = Group::getQuery();
            ResultQuery *result = context->executeQuery(query);

            if (result != nullptr && result->isValid()) {
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Groups getted");
                std::map<int, std::shared_ptr<Group>>::iterator it;
                std::map<int, std::shared_ptr<Group>> groups = Group::loadListFromResult(result);

                // -- build string out
                std::string datas = "<groups>";
                for (it = groups.begin(); it != groups.end(); it++) {datas += it->second->toXmlString(false);}
                datas += "</groups>";
                response->setDatas(datas);
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Groups get failed");
                response->setDatas("<error><code>" + std::to_string(result->getErrorCode()) + "</code><message>" + result->getErrorMessage() + "</message></error>");
            }
            delete query;
            delete result;
        }

        // -- get all datas of one particular group
        else {
            Query* queryGroup = Group::getQuery(&id);
            ResultQuery *resultGroup = context->executeQuery(queryGroup);
            Query* queryCinema = Cinema::getQuery();
            ResultQuery *resultCinema = context->executeQuery(queryCinema);
            Query* queryRelease = Release::getQuery();
            ResultQuery *resultRelease = context->executeQuery(queryRelease);

            Query* queryLinkCinema = LinkParam::getQueryDst(LinkParam::CINEMA_GROUP, &id);
            ResultQuery *resultLinkCinema = context->executeQuery(queryLinkCinema);
            Query* queryLinkRelease = LinkParam::getQuerySrc(LinkParam::GROUP_RELEASE, &id);
            ResultQuery *resultLinkRelease = context->executeQuery(queryLinkRelease);

            if (!resultGroup->isValid() || !resultCinema->isValid() || !resultLinkCinema->isValid()
                || !resultRelease->isValid() || !resultLinkRelease->isValid()) {

                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Group get failed");
                if (!resultGroup->isValid()) {response->setDatas("<error><code>" + std::to_string(resultGroup->getErrorCode()) + "</code><message>" + resultGroup->getErrorMessage() + "</message></error>");}
                else if (!resultCinema->isValid()) {response->setDatas("<error><code>" + std::to_string(resultCinema->getErrorCode()) + "</code><message>" + resultCinema->getErrorMessage() + "</message></error>");}
                else if (!resultRelease->isValid()) {response->setDatas("<error><code>" + std::to_string(resultRelease->getErrorCode()) + "</code><message>" + resultRelease->getErrorMessage() + "</message></error>");}
                else if (!resultLinkCinema->isValid()) {response->setDatas("<error><code>" + std::to_string(resultLinkCinema->getErrorCode()) + "</code><message>" + resultLinkCinema->getErrorMessage() + "</message></error>");}
                else if (!resultLinkRelease->isValid()) {response->setDatas("<error><code>" + std::to_string(resultLinkRelease->getErrorCode()) + "</code><message>" + resultLinkRelease->getErrorMessage() + "</message></error>");}
            }
            else {
                std::shared_ptr<Group> group = Group::loadFromResult(resultGroup);
                std::map<int, std::shared_ptr<Cinema>> cinemas = Cinema::loadListFromResult(resultCinema);
                std::map<int, std::shared_ptr<Release>> releases = Release::loadListFromResult(resultRelease);
                std::vector<std::shared_ptr<LinkParam>> linkCinemas = LinkParam::loadListFromResult(resultLinkCinema, LinkParam::CINEMA_GROUP);
                std::vector<std::shared_ptr<LinkParam>> linkReleases = LinkParam::loadListFromResult(resultLinkRelease, LinkParam::GROUP_RELEASE);

                for (std::shared_ptr<LinkParam> link : linkCinemas) {
                    std::map<int, std::shared_ptr<Cinema>>::iterator it = cinemas.find(link->getSrcId()); 
                    if (it != cinemas.end()) {group->addCinema(it->second);}
                }
                for (std::shared_ptr<LinkParam> link : linkReleases) {
                    std::map<int, std::shared_ptr<Release>>::iterator it = releases.find(link->getDstId()); 
                    if (it != releases.end()) {group->addRelease(it->second);}
                }

                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Group getted");
                if (group) {
                    response->setDatas(group->toXmlString(true));
                } else {
                    response->setDatas("");
                }
            }

            delete queryGroup;
            delete queryCinema;
            delete queryRelease;
            delete queryLinkCinema;
            delete queryLinkRelease;
            delete resultGroup;
            delete resultCinema;
            delete resultRelease;
            delete resultLinkCinema;
            delete resultLinkRelease;
        }
    }
    else if (cmd->getType() == CommandCentral::GET_CINEMAS) {

       int id = cmd->getIntParameter("id");

        if (id == -1) {
            Query* query = Cinema::getQuery();
            ResultQuery *result = context->executeQuery(query);

            if (result != nullptr && result->isValid()) {
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Cinemas getted");
                std::map<int, std::shared_ptr<Cinema>>::iterator it;
                std::map<int, std::shared_ptr<Cinema>> cinemas = Cinema::loadListFromResult(result);

                // -- build string out
                std::string datas = "<cinemas>";
                for (it = cinemas.begin(); it != cinemas.end(); it++) {datas += it->second->toXmlString(false);}
                datas += "</cinemas>";
                response->setDatas(datas);
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Cinemas get failed");
            }
            delete query;
            delete result;
        }

        // -- get all datas of one particular group
        else {
            Query* queryCinema = Cinema::getQuery(&id);
            ResultQuery *resultCinema = context->executeQuery(queryCinema);
            Query* queryAudis = Auditorium::getQueryForCinema(&id);
            ResultQuery *resultAudis = context->executeQuery(queryAudis);

            if (!resultCinema->isValid() || !resultAudis->isValid()) {

                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Cinema get failed");
            }
            else {
                std::shared_ptr<Cinema> cinema = Cinema::loadFromResult(resultCinema);
                std::map<int, std::shared_ptr<Auditorium>> audis = Auditorium::loadListFromResult(resultAudis);
                std::map<int, std::shared_ptr<Auditorium>>::iterator it;

                for (it = audis.begin(); it != audis.end(); it++) {
                    cinema->addAuditorium(it->second);
                }

                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Cinema getted");
                response->setDatas(cinema->toXmlString(true));
            }

            delete queryCinema;
            delete queryAudis;
            delete resultCinema;
            delete resultAudis;
        }
    }
    else if (cmd->getType() == CommandCentral::GET_AUDITORIUMS) {
        
        int id = cmd->getIntParameter("id");

        if (id == -1) {
            Query* query = Auditorium::getQuery();
            ResultQuery *result = context->executeQuery(query);

            if (result != nullptr && result->isValid()) {
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Auditoriums getted");
                std::map<int, std::shared_ptr<Auditorium>>::iterator it;
                std::map<int, std::shared_ptr<Auditorium>> auditoriums = Auditorium::loadListFromResult(result);

                // -- build string out
                std::string datas = "<auditoriums>";
                for (it = auditoriums.begin(); it != auditoriums.end(); it++) {datas += it->second->toXmlString(false);}
                datas += "</auditoriums>";
                response->setDatas(datas);
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Auditoriums get failed");
            }
            delete query;
            delete result;
        }

        // -- get all datas of one particular group
        else {
            Query* queryAudis = Auditorium::getQuery(&id);
            ResultQuery *resultAudis = context->executeQuery(queryAudis);
            Query* queryServers = Server::getQueryForAuditorium(&id);
            ResultQuery *resultServers = context->executeQuery(queryServers);

            if (!resultAudis->isValid() || !resultServers->isValid()) {

                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Auditorium get failed");
                if (!resultAudis->isValid()) {response->setDatas("<error><code>" + std::to_string(resultAudis->getErrorCode()) + "</code><message>" + resultAudis->getErrorMessage() + "</message></error>");}
                else if (!resultServers->isValid()) {response->setDatas("<error><code>" + std::to_string(resultServers->getErrorCode()) + "</code><message>" + resultServers->getErrorMessage() + "</message></error>");}
            }
            else {
                std::shared_ptr<Auditorium> audi = Auditorium::loadFromResult(resultAudis);
                std::map<int, std::shared_ptr<Server>> servers = Server::loadListFromResult(resultServers);
                std::map<int, std::shared_ptr<Server>>::iterator it;

                for (it = servers.begin(); it != servers.end(); it++) {
                    if (it->second->getTypeServer() == Server::FEATURE) {
                        audi->setFeatureServer(it->second);
                    }
                    else if (it->second->getTypeServer() == Server::SAS) {
                        audi->setSasServer(it->second);
                    }
                }

                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Auditorium getted");
                response->setDatas(audi->toXmlString(true));
            }

            delete queryAudis;
            delete queryServers;
            delete resultAudis;
            delete resultServers;
        }
    }
    else if (cmd->getType() == CommandCentral::GET_SERVERS) {
        
        int id = cmd->getIntParameter("id");
        int id_auditorium = cmd->getIntParameter("id_auditorium");

        if (id_auditorium == -1) {

            Query* query; 
            if (id == -1) {query = Server::getQuery(nullptr);}
            else {query = Server::getQuery(&id);}
            ResultQuery* result = context->executeQuery(query);

            if (result->isValid())
            {
                std::string datas = "";
                if (id == -1) {
                    std::map<int, std::shared_ptr<Server>>::iterator it;
                    std::map<int, std::shared_ptr<Server>> servers = Server::loadListFromResult(result);
                    datas = "<servers>";
                    for (it = servers.begin(); it != servers.end(); it++) {
                        datas += it->second->toXmlString();
                    }
                    datas += "</servers>";
                }
                else {
                    std::shared_ptr<Server> server = Server::loadFromResult(result);
                    datas = server->toXmlString();
                }

                response->setDatas(datas);
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("GET_SERVERS achieved !");
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("GET_SERVERS failed !");
            }

            delete query;
            delete result;
        }
        else {
            Query* query = Server::getQueryForAuditorium(&id_auditorium);
            ResultQuery* result = context->executeQuery(query);

            if (result->isValid())
            {
                std::string datas = "";
                if (id == -1) {
                    std::map<int, std::shared_ptr<Server>>::iterator it;
                    std::map<int, std::shared_ptr<Server>> servers = Server::loadListFromResult(result);
                    datas = "<servers>";
                    for (it = servers.begin(); it != servers.end(); it++) {
                        datas += it->second->toXmlString();
                    }
                    datas += "</servers>";
                }
                else {
                    std::shared_ptr<Server> server = Server::loadFromResult(result);
                    datas = server->toXmlString();
                }

                response->setDatas(datas);
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("GET_SERVERS achieved !");
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("GET_SERVERS failed !");
            }

            delete query;
            delete result;
        }
    }
    else if (cmd->getType() == CommandCentral::GET_SCRIPTS) {
        
        int id = cmd->getIntParameter("id");

        if (id == -1) {
            Query* query = Script::getQuery();
            ResultQuery* result = context->executeQuery(query);

            if (result->isValid())
            {
                std::string datas = "";
                if (id == -1) {
                    std::map<int, std::shared_ptr<Script>>::iterator it;
                    std::map<int, std::shared_ptr<Script>> scripts = Script::loadListFromResult(result);
                    datas = "<scripts>";
                    for (it = scripts.begin(); it != scripts.end(); it++) {
                        datas += it->second->toXmlString();
                    }
                    datas += "</scripts>";
                }
                else {
                    std::shared_ptr<Script> script = Script::loadFromResult(result);
                    datas = script->toXmlString();
                }

                response->setDatas(datas);
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("GET_SCRIPTS achieved !");
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("GET_SCRIPTS failed !");
            }

            delete query;
            delete result;
        }
        else {
            Query* query = Script::getQuery(&id);
            ResultQuery* result = context->executeQuery(query);

            if (result->isValid())
            {
                std::string datas = "";
                if (id == -1) {
                    std::map<int, std::shared_ptr<Script>>::iterator it;
                    std::map<int, std::shared_ptr<Script>> scripts = Script::loadListFromResult(result);
                    datas = "<scripts>";
                    for (it = scripts.begin(); it != scripts.end(); it++) {
                        datas += it->second->toXmlString();
                    }
                    datas += "</scripts>";
                }
                else {
                    std::shared_ptr<Script> script = Script::loadFromResult(result);
                    datas = script->toXmlString();
                }

                response->setDatas(datas);
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("GET_SCRIPTS achieved !");
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("GET_SCRIPTS failed !");
            }

            delete query;
            delete result;
        }
    }
    else if (cmd->getType() == CommandCentral::GET_FEATURES) {

        int id = cmd->getIntParameter("id");

        if (id == -1) {
            Query* query = Feature::getQuery();
            ResultQuery *result = context->executeQuery(query);

            if (result != nullptr && result->isValid()) {
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Features getted");
                std::map<int, std::shared_ptr<Feature>>::iterator it;
                std::map<int, std::shared_ptr<Feature>> features = Feature::loadListFromResult(result);

                // -- build string out
                std::string datas = "<features>";
                for (it = features.begin(); it != features.end(); it++) {datas += it->second->toXmlString(false);}
                datas += "</features>";
                response->setDatas(datas);
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Features get failed");
            }
            delete query;
            delete result;
        }

        // -- get all datas of one particular group
        else {
            Query* queryFeature = Feature::getQuery(&id);
            ResultQuery *resultFeature = context->executeQuery(queryFeature);
            Query* queryReleases = Release::getQueryForFeature(&id);
            ResultQuery *resultReleases = context->executeQuery(queryReleases);

            if (!resultFeature->isValid() || !resultReleases->isValid()) {

                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Feature get failed");
            }
            else {
                std::shared_ptr<Feature> feature = Feature::loadFromResult(resultFeature);
                std::map<int, std::shared_ptr<Release>> releases = Release::loadListFromResult(resultReleases);
                std::map<int, std::shared_ptr<Release>>::iterator it;

                for (it = releases.begin(); it != releases.end(); it++) {
                    feature->addRelease(it->second);
                }

                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Feature getted");
                response->setDatas(feature->toXmlString(true));
            }

            delete queryFeature;
            delete queryReleases;
            delete resultFeature;
            delete resultReleases;
        }
    }
    else if (cmd->getType() == CommandCentral::GET_RELEASES) {

        int id = cmd->getIntParameter("id");
        int id_feature = cmd->getIntParameter("id_feature");

        if (id == -1) {
            Query* query;
            
            if (id_feature == -1) {query = Release::getQuery();}
            else {query = Release::getQueryForFeature(&id_feature);}
            ResultQuery *result = context->executeQuery(query);

            if (result != nullptr && result->isValid()) {
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Releases getted");
                std::map<int, std::shared_ptr<Release>>::iterator it;
                std::map<int, std::shared_ptr<Release>> release = Release::loadListFromResult(result);

                // -- build string out
                std::string datas = "<releases>";
                for (it = release.begin(); it != release.end(); it++) {datas += it->second->toXmlString(false);}
                datas += "</releases>";
                response->setDatas(datas);
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Releases get failed");
            }
            delete query;
            delete result;
        }

        // -- get all datas of one particular group
        else {
            Query* queryRelease = Release::getQuery(&id);
            ResultQuery *resultRelease = context->executeQuery(queryRelease);
            Query* queryCuts = ReleaseCut::getQueryForRelease(&id);
            ResultQuery *resultCuts = context->executeQuery(queryCuts);
            Query* queryScript = Script::getQuery();
            ResultQuery *resultScript = context->executeQuery(queryScript);
            Query* queryCinemas = Cinema::getQuery();
            ResultQuery *resultCinemas = context->executeQuery(queryCinemas);
            Query* queryCpls = Cpl::getQuery();
            ResultQuery *resultCpls = context->executeQuery(queryCpls);
            Query* queryLinkCplRelease = LinkParam::getQueryDst(LinkParam::CPL_RELEASE, &id);
            ResultQuery *resultLinkCplRelease = context->executeQuery(queryLinkCplRelease);
            Query* queryLinkCinemaRelease = LinkParam::getQueryDst(LinkParam::CINEMA_RELEASE, &id);
            ResultQuery *resultLinkCinemaRelease = context->executeQuery(queryLinkCinemaRelease);
            Query* queryLinkScriptRelease = LinkParam::getQueryDst(LinkParam::SCRIPT_RELEASE, &id);
            ResultQuery *resultLinkScriptRelease = context->executeQuery(queryLinkScriptRelease);

            if (!resultRelease->isValid() || !resultCuts->isValid() ||
                !resultScript->isValid() || !resultCinemas->isValid() || !resultCpls->isValid() || 
                !resultLinkCplRelease->isValid() || !resultLinkCinemaRelease->isValid() ||
                !resultLinkScriptRelease->isValid()) {

                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Release get failed");
                if (!resultRelease->isValid()) {response->setDatas("<error>release<code>" + std::to_string(resultRelease->getErrorCode()) + "</code><message>" + resultRelease->getErrorMessage() + "</message></error>");}
                else if (!resultCuts->isValid()) {response->setDatas("<error>cuts<code>" + std::to_string(resultCuts->getErrorCode()) + "</code><message>" + resultCuts->getErrorMessage() + "</message></error>");}
                else if (!resultScript->isValid()) {response->setDatas("<error>script<code>" + std::to_string(resultScript->getErrorCode()) + "</code><message>" + resultScript->getErrorMessage() + "</message></error>");}
                else if (!resultCinemas->isValid()) {response->setDatas("<error>cinemas<code>" + std::to_string(resultCinemas->getErrorCode()) + "</code><message>" + resultCinemas->getErrorMessage() + "</message></error>");}
                else if (!resultCpls->isValid()) {response->setDatas("<error>cpl<code>" + std::to_string(resultCpls->getErrorCode()) + "</code><message>" + resultCpls->getErrorMessage() + "</message></error>");}
                else if (!resultLinkCplRelease->isValid()) {response->setDatas("<error>linkCpl<code>" + std::to_string(resultLinkCplRelease->getErrorCode()) + "</code><message>" + resultLinkCplRelease->getErrorMessage() + "</message></error>");}
                else if (!resultLinkCinemaRelease->isValid()) {response->setDatas("<error>linkCinema<code>" + std::to_string(resultLinkCinemaRelease->getErrorCode()) + "</code><message>" + resultLinkCinemaRelease->getErrorMessage() + "</message></error>");}
                else if (!resultLinkScriptRelease->isValid()) {response->setDatas("<error>linkScript<code>" + std::to_string(resultLinkScriptRelease->getErrorCode()) + "</code><message>" + resultLinkScriptRelease->getErrorMessage() + "</message></error>");}
            }
            else {
                std::shared_ptr<Release> release = Release::loadFromResult(resultRelease);
                std::map<int, std::shared_ptr<ReleaseCut>> cuts = ReleaseCut::loadListFromResult(resultCuts);
                std::map<int, std::shared_ptr<ReleaseCut>>::iterator itCut;
                std::map<int, std::shared_ptr<Script>> scripts = Script::loadListFromResult(resultScript);
                std::map<int, std::shared_ptr<Script>>::iterator itScript;
                std::map<int, std::shared_ptr<Cpl>> cpls = Cpl::loadListFromResult(resultCpls);
                std::map<int, std::shared_ptr<Cpl>>::iterator itCpl;
                std::map<int, std::shared_ptr<Cinema>> cinemas = Cinema::loadListFromResult(resultCinemas);
                std::map<int, std::shared_ptr<Cinema>>::iterator itCinema;
                std::vector<std::shared_ptr<LinkParam>> links = LinkParam::loadListFromResult(resultLinkCplRelease, LinkParam::CPL_RELEASE);
                std::vector<std::shared_ptr<LinkParam>> linksCinema = LinkParam::loadListFromResult(resultLinkCinemaRelease, LinkParam::CINEMA_RELEASE);
                std::vector<std::shared_ptr<LinkParam>> linksScript = LinkParam::loadListFromResult(resultLinkScriptRelease, LinkParam::SCRIPT_RELEASE);

                for (itCut = cuts.begin(); itCut != cuts.end(); itCut++) {
                    release->addCut(itCut->second);
                }

                for (std::shared_ptr<LinkParam> link : linksScript) {
                    itScript = scripts.find(link->getSrcId());
                    if (itScript != scripts.end()) {
                        if (itScript->second->getType() == Script::FEATURE) {
                            release->setFeatureScript(itScript->second);
                        }
                        else if (itScript->second->getType() == Script::LOOP) {
                            release->setLoopScript(itScript->second);
                        }
                        else if (itScript->second->getType() == Script::SAS) {
                            release->setSasScript(itScript->second);
                        }
                    }
                }

                for (std::shared_ptr<LinkParam> link : links) {
                    itCpl = cpls.find(link->getSrcId()); 
                    if (itCpl != cpls.end()) {release->addFeatureCpl(itCpl->second);}
                }

                for (std::shared_ptr<LinkParam> link : linksCinema) {
                    itCinema = cinemas.find(link->getSrcId()); 
                    if (itCinema != cinemas.end()) {release->addCinema(itCinema->second);}
                }

                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Release getted");
                response->setDatas(release->toXmlString(true));
            }

            delete queryRelease;
            delete queryCuts;
            delete queryScript;
            delete queryCpls;
            delete queryCinemas;
            delete queryLinkCplRelease;
            delete queryLinkCinemaRelease;
            delete queryLinkScriptRelease;
            delete resultRelease;
            delete resultCuts;
            delete resultScript;
            delete resultCpls;
            delete resultCinemas;
            delete resultLinkCplRelease;
            delete resultLinkCinemaRelease;
            delete resultLinkScriptRelease;
        }
    }
    else if (cmd->getType() == CommandCentral::GET_CPLS) {
        
        int id = cmd->getIntParameter("id");
        int id_script = cmd->getIntParameter("id_script");
        int id_release = cmd->getIntParameter("id_release");
        std::string uuid = cmd->getStringParameter("uuid");

        try {
            std::string datas;
            
            if (id != -1) {
                datas = BoundaryManager::GetInstance().GetCplAsXml(id);
                response->setComments("GET_CPL achieved !");
            }
            else if (!uuid.empty() && uuid != "-1") {
                datas = BoundaryManager::GetInstance().GetCplAsXmlByUuid(uuid);
                response->setComments("GET_CPL by UUID achieved !");
            }
            else if (id_script != -1) {
                datas = BoundaryManager::GetInstance().GetCplsByScriptAsXml(id_script);
                response->setComments("GET_CPLS for script achieved !");
            }
            else if (id_release != -1) {
                datas = BoundaryManager::GetInstance().GetCplsByReleaseAsXml(id_release);
                response->setComments("GET_CPLS for release achieved !");
            }
            else {
                datas = BoundaryManager::GetInstance().GetUnlinkedCplsAsXml();
                response->setComments("GET_CPLS for unlinked achieved !");
            }
            
            response->setDatas(datas);
            response->setStatus(CommandCentralResponse::OK);
        }
        catch(const std::exception& e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("GET_CPLS failed: " + std::string(e.what()));
        }
    }
    else if (cmd->getType() == CommandCentral::GET_CUT) {
        int id = cmd->getIntParameter("id");

        if (id == -1) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Cut get failed");
        }

        // -- get all datas of one particular group
        else {
            Query* query = ReleaseCut::getQuery(&id);
            ResultQuery *result = context->executeQuery(query);

            if (!result->isValid()) {

                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Cut get failed");
            }
            else {
                std::shared_ptr<ReleaseCut> cut = ReleaseCut::loadFromResult(result);
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Cut getted");
                response->setDatas(cut->toXmlString());
            }

            delete query;
            delete result;
        }
    }

    // -- INSERT
    else if (cmd->getType() == CommandCentral::INSERT_GROUP) {

        Group group;
        group.setDatas(cmd->getStringParameter("name"), cmd->getStringParameter("description"));
        Query* query = group.createQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Group created");
            response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Group create failed");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::INSERT_CINEMA) {

        Cinema cinema;
        cinema.setDatas(cmd->getStringParameter("name"));
        Query* query = cinema.createQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Cinema created");
            response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Cinema create failed");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::INSERT_AUDITORIUM) {

        int id_cinema = cmd->getIntParameter("id_cinema");
        Query* queryGetCinema = Cinema::getQuery(&id_cinema);
        ResultQuery *resultGetCinema = context->executeQuery(queryGetCinema);

        if (resultGetCinema->isValid())
        {
            std::shared_ptr<Cinema> cinema = Cinema::loadFromResult(resultGetCinema);
            if (cinema == nullptr) {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Cinema referenced not found !");
            }
            else {

                Auditorium audi;
                audi.setDatas(cmd->getIntParameter("id_cinema"), cmd->getStringParameter("name"), cmd->getIntParameter("room"));
                audi.setIms((Auditorium::TypeIms)cmd->getIntParameter("type_ims"), cmd->getStringParameter("ip_ims"), cmd->getIntParameter("port_ims"), cmd->getStringParameter("user_ims"), cmd->getStringParameter("pass_ims"));
                Query* query = audi.createQuery();
                ResultQuery *result = context->executeQuery(query);

                if (result->isValid()) {

                    response->setStatus(CommandCentralResponse::OK);
                    response->setComments("Auditorium created");
                    response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
                }
                else {
                    response->setStatus(CommandCentralResponse::KO);
                    response->setComments("Auditorium create failed");
                }

                delete query;
                delete result;
            }
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Cinema referenced could not be getted !");
        }

        delete queryGetCinema;
        delete resultGetCinema;
    }
    else if (cmd->getType() == CommandCentral::INSERT_SERVER) {

        int id_auditorium = cmd->getIntParameter("id_auditorium");
        Query* queryGetAuditorium = Auditorium::getQuery(&id_auditorium);
        ResultQuery *resultGetAuditorium = context->executeQuery(queryGetAuditorium);

        if (resultGetAuditorium->isValid())
        {
            std::shared_ptr<Auditorium> audi = Auditorium::loadFromResult(resultGetAuditorium);
            if (audi == nullptr) {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Auditorium referenced not found !");
            }
            else {

                Server server;
                server.setDatas(id_auditorium, (Server::TypeServer)cmd->getIntParameter("type_server"), cmd->getStringParameter("ip"));
                Query* query = server.createQuery();
                ResultQuery *result = context->executeQuery(query);

                if (result->isValid()) {

                    response->setStatus(CommandCentralResponse::OK);
                    response->setComments("Server created");
                    response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
                }
                else {
                    response->setStatus(CommandCentralResponse::KO);
                    response->setComments("Server create failed");
                }

                delete query;
                delete result;
            }
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Auditorium referenced could not be getted !");
        }

        delete queryGetAuditorium;
        delete resultGetAuditorium;
    }
    else if (cmd->getType() == CommandCentral::INSERT_SCRIPT) {

        std::string sha1Lvi = Converter::calculateSha1OfFile(cmd->getStringParameter("path") + "/" + cmd->getStringParameter("lvi_name"));
        Script script;
        script.setDatas(cmd->getStringParameter("name"), cmd->getStringParameter("cis_name"), cmd->getStringParameter("lvi_name"), sha1Lvi);
        script.setLink((Script::ScriptType)cmd->getIntParameter("type"), cmd->getStringParameter("path"), cmd->getStringParameter("version"));
        Query* query = script.createQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result->isValid()) {

            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Script created");
            response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Script create failed");
        }

        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::INSERT_CPL) {

        Poco::File pathSync = cmd->getStringParameter("path_sync");
        std::string sha1Sync = "";
        if (pathSync.exists()) {
            sha1Sync = Converter::calculateSha1OfFile(cmd->getStringParameter("path_sync"));
        }
        Cpl cpl;
        cpl.setDatas(cmd->getStringParameter("uuid"), cmd->getStringParameter("name"));
        cpl.setCplInfos(cmd->getStringParameter("path_cpl"), cmd->getStringParameter("path_sync"), sha1Sync, (Cpl::CplType)cmd->getIntParameter("type"));
        Query* query = cpl.createQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result->isValid()) {

            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Cpl created");
            response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Cpl create failed");
        }

        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::INSERT_FEATURE) {

        Feature feature;
        feature.setDatas(cmd->getStringParameter("name"));
        Query* query = feature.createQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result->isValid()) {

            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Feature created");
            response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Feature create failed");
        }

        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::INSERT_RELEASE) {
        
        // -- check and parse date
        std::string date_inStr = cmd->getStringParameter("date_in");
        std::string date_outStr = cmd->getStringParameter("date_out");
        bool isValid = true;
        Poco::DateTime date_in;
        Poco::DateTime date_out;
        std::shared_ptr<Poco::DateTime> in = nullptr;
        std::shared_ptr<Poco::DateTime> out = nullptr;

        if (date_inStr.empty()) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("INSERT_RELEASE failed : date_in can't be empty !");
            isValid = false;
        }
        else {
            // -- parse date
            
            try {
                int pos1 = date_inStr.find("/", 0);
                int pos2 = date_inStr.find("/", pos1+1);
                int year = std::stoi(date_inStr.substr(0, pos1));
                int month = std::stoi(date_inStr.substr(pos1+1, pos2-pos1));
                int day = std::stoi(date_inStr.substr(pos2+1));
                date_in = Poco::DateTime(year, month, day);
                in = std::make_shared<Poco::DateTime>(date_in);
            }
            catch (std::exception &e) {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("INSERT_RELEASE failed : date_in parsed failed !");
                isValid = false;
            }

            if (!date_outStr.empty()) {
                try {
                    int pos1 = date_outStr.find("/", 0);
                    int pos2 = date_outStr.find("/", pos1+1);
                    int year = std::stoi(date_outStr.substr(0, pos1));
                    int month = std::stoi(date_outStr.substr(pos1+1, pos2-pos1));
                    int day = std::stoi(date_outStr.substr(pos2+1));
                    date_out = Poco::DateTime(year, month, day);
                    out = std::make_shared<Poco::DateTime>(date_out);
                }
                catch (std::exception &e) {
                    response->setStatus(CommandCentralResponse::KO);
                    response->setComments("INSERT_RELEASE failed : date_out parsed failed !");
                    isValid = false;
                }
            }
        }

        if (isValid) {

            int id_feature = cmd->getIntParameter("id_feature");
            Query* queryGetFeature = Feature::getQuery(&id_feature);
            ResultQuery *resultGetFeature = context->executeQuery(queryGetFeature);

            if (resultGetFeature->isValid())
            {
                std::shared_ptr<Feature> feature = Feature::loadFromResult(resultGetFeature);
                if (feature == nullptr) {
                    response->setStatus(CommandCentralResponse::KO);
                    response->setComments("Feature referenced not found !");
                }
                else {

                    Release release;
                    release.setDatas(cmd->getIntParameter("id_feature"), cmd->getStringParameter("name"));
                    release.setDate(in, out);
                    release.setPattern(cmd->getStringParameter("global_pattern"), cmd->getStringParameter("specific_pattern"));
                    release.setFinalized(cmd->getBoolParameter("finalized"));
                    release.setParent(cmd->getIntParameter("id_parent_release"));
                    Query* query = release.createQuery();
                    ResultQuery *result = context->executeQuery(query);

                    if (result->isValid()) {

                        response->setStatus(CommandCentralResponse::OK);
                        response->setComments("Release created");
                        response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
                    }
                    else {
                        response->setStatus(CommandCentralResponse::KO);
                        response->setComments("Release create failed");
                        response->setDatas("<error><code>" + std::to_string(result->getErrorCode()) + "</code><message>" + result->getErrorMessage() + "</message></error>");
                    }

                    delete query;
                    delete result;
                }
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Feature referenced could not be getted !");
            }

            delete queryGetFeature;
            delete resultGetFeature;
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Date format is invalid !");
        }
    }
    else if (cmd->getType() == CommandCentral::INSERT_CUT) {
        ReleaseCut cut;
        cut.setDatas(cmd->getStringParameter("description"), cmd->getIntParameter("position"), cmd->getIntParameter("size"));
        cut.setIdRelease(cmd->getIntParameter("id_release"));
        Query* query = cut.createQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Cut created");
            response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Cut create failed");
        }
        delete query;
        delete result;
    }

    // -- UPDATE
    else if (cmd->getType() == CommandCentral::UPDATE_GROUP) {
        Group group;
        group.setId(cmd->getIntParameter("id"));
        group.setDatas(cmd->getStringParameter("name"), cmd->getStringParameter("description"));
        Query* query = group.updateQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Group updated");
            response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Group update failed");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::UPDATE_CINEMA) {
        
        Cinema cinema;
        cinema.setId(cmd->getIntParameter("id"));
        cinema.setDatas(cmd->getStringParameter("name"));
        Query* query = cinema.updateQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Cinema updated");
            response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Cinema update failed");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::UPDATE_AUDITORIUM) {

        int id_cinema = cmd->getIntParameter("id_cinema");
        Query* queryGetCinema = Cinema::getQuery(&id_cinema);
        ResultQuery *resultGetCinema = context->executeQuery(queryGetCinema);

        if (resultGetCinema->isValid())
        {
            std::shared_ptr<Cinema> cinema = Cinema::loadFromResult(resultGetCinema);
            if (cinema == nullptr) {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Cinema referenced not found !");
            }
            else {

                Auditorium audi;
                audi.setId(cmd->getIntParameter("id"));
                audi.setDatas(cmd->getIntParameter("id_cinema"), cmd->getStringParameter("name"), cmd->getIntParameter("room"));
                audi.setIms((Auditorium::TypeIms)cmd->getIntParameter("type_ims"), cmd->getStringParameter("ip_ims"), cmd->getIntParameter("port_ims"), cmd->getStringParameter("user_ims"), cmd->getStringParameter("pass_ims"));
                Query* query = audi.updateQuery();
                ResultQuery *result = context->executeQuery(query);

                if (result->isValid()) {

                    response->setStatus(CommandCentralResponse::OK);
                    response->setComments("Auditorium created");
                    response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
                }
                else {
                    response->setStatus(CommandCentralResponse::KO);
                    response->setComments("Auditorium create failed");
                }

                delete query;
                delete result;
            }
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Cinema referenced could not be getted !");
        }

        delete queryGetCinema;
        delete resultGetCinema;
    }
    else if (cmd->getType() == CommandCentral::UPDATE_SERVER) {

        int id_auditorium = cmd->getIntParameter("id_auditorium");
        Query* queryGetAuditorium = Auditorium::getQuery(&id_auditorium);
        ResultQuery *resultGetAuditorium = context->executeQuery(queryGetAuditorium);

        if (resultGetAuditorium->isValid())
        {
            std::shared_ptr<Auditorium> audi = Auditorium::loadFromResult(resultGetAuditorium);
            if (audi == nullptr) {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Auditorium referenced not found !");
            }
            else {

                Server server;
                server.setId(cmd->getIntParameter("id"));
                server.setDatas(cmd->getIntParameter("id_auditorium"), (Server::TypeServer)cmd->getIntParameter("type_server"), cmd->getStringParameter("ip"));
                Query* query = server.updateQuery();
                ResultQuery *result = context->executeQuery(query);

                if (result->isValid()) {

                    response->setStatus(CommandCentralResponse::OK);
                    response->setComments("Server created");
                    response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
                }
                else {
                    response->setStatus(CommandCentralResponse::KO);
                    response->setComments("Server create failed");
                }

                delete query;
                delete result;
            }
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Auditorium referenced could not be getted !");
        }

        delete queryGetAuditorium;
        delete resultGetAuditorium;
    }
    else if (cmd->getType() == CommandCentral::UPDATE_SCRIPT) {
        
        Script script;
        script.setId(cmd->getIntParameter("id"));
        script.setDatas(cmd->getStringParameter("name"), cmd->getStringParameter("cis_name"), cmd->getStringParameter("lvi_name"),cmd->getStringParameter("sha1Lvi"));
        script.setLink((Script::ScriptType)cmd->getIntParameter("type"), cmd->getStringParameter("path"), cmd->getStringParameter("version"));
        Query* query = script.updateQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result->isValid()) {

            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Script updated");
            response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Script update failed");
        }

        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::UPDATE_CPL) {

        Cpl cpl;
        cpl.setId(cmd->getIntParameter("id"));
        cpl.setDatas(cmd->getStringParameter("uuid"), cmd->getStringParameter("name"));
        cpl.setCplInfos(cmd->getStringParameter("path_cpl"), cmd->getStringParameter("path_sync"), cmd->getStringParameter("sha1_sync"), (Cpl::CplType)cmd->getIntParameter("type"));
        Query* query = cpl.updateQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result->isValid()) {

            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Cpl updated");
            response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Cpl update failed");
        }

        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::UPDATE_FEATURE) {

        Feature feature;
        feature.setId(cmd->getIntParameter("id"));
        feature.setDatas(cmd->getStringParameter("name"));
        Query* query = feature.updateQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result->isValid()) {

            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Feature updated");
            response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Feature update failed");
        }

        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::UPDATE_RELEASE) {

         // -- check and parse date
        std::string date_inStr = cmd->getStringParameter("date_in");
        std::string date_outStr = cmd->getStringParameter("date_out");
        bool isValid = true;
        Poco::DateTime date_in;
        Poco::DateTime date_out;
        std::shared_ptr<Poco::DateTime> in = nullptr;
        std::shared_ptr<Poco::DateTime> out = nullptr;

        if (date_inStr.empty()) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("INSERT_RELEASE failed : date_in can't be empty !");
            isValid = false;
        }
        else {
            // -- parse date
            
            try {
                int pos1 = date_inStr.find("/", 0);
                int pos2 = date_inStr.find("/", pos1+1);
                int year = std::stoi(date_inStr.substr(0, pos1));
                int month = std::stoi(date_inStr.substr(pos1+1, pos2-pos1));
                int day = std::stoi(date_inStr.substr(pos2+1));
                date_in = Poco::DateTime(year, month, day);
                in = std::make_shared<Poco::DateTime>(date_in);
            }
            catch (std::exception &e) {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("INSERT_RELEASE failed : date_in parsed failed !");
                isValid = false;
            }

            if (!date_outStr.empty()) {
                try {
                    int pos1 = date_outStr.find("/", 0);
                    int pos2 = date_outStr.find("/", pos1+1);
                    int year = std::stoi(date_outStr.substr(0, pos1));
                    int month = std::stoi(date_outStr.substr(pos1+1, pos2-pos1));
                    int day = std::stoi(date_outStr.substr(pos2+1));
                    date_out = Poco::DateTime(year, month, day);
                    out = std::make_shared<Poco::DateTime>(date_out);
                }
                catch (std::exception &e) {
                    response->setStatus(CommandCentralResponse::KO);
                    response->setComments("INSERT_RELEASE failed : date_out parsed failed !");
                    isValid = false;
                }
            }
        }

        if (isValid) {

            int id_release = cmd->getIntParameter("id_feature");
            Query* queryGetFeature = Feature::getQuery(&id_release);
            ResultQuery *resultGetFeature = context->executeQuery(queryGetFeature);

            if (resultGetFeature->isValid())
            {
                std::shared_ptr<Feature> feature = Feature::loadFromResult(resultGetFeature);
                if (feature == nullptr) {
                    response->setStatus(CommandCentralResponse::KO);
                    response->setComments("Feature referenced not found !");
                }
                else {

                    Release release;
                    release.setId(cmd->getIntParameter("id"));
                    release.setDatas(cmd->getIntParameter("id_feature"), cmd->getStringParameter("name"));
                    release.setDate(in, out);
                    release.setPattern(cmd->getStringParameter("global_pattern"), cmd->getStringParameter("specific_pattern"));
                    release.setFinalized(cmd->getBoolParameter("finalized"));
                    release.setParent(cmd->getIntParameter("id_parent_release"));
                    Query* query = release.updateQuery();
                    ResultQuery *result = context->executeQuery(query);

                    if (result->isValid()) {

                        response->setStatus(CommandCentralResponse::OK);
                        response->setComments("Release updated");
                        response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
                    }
                    else {
                        response->setStatus(CommandCentralResponse::KO);
                        response->setComments("Release update failed");
                    }

                    delete query;
                    delete result;
                }
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Feature referenced could not be getted !");
            }

            delete queryGetFeature;
            delete resultGetFeature;
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Date format is invalid !");
        }
    }
    else if (cmd->getType() == CommandCentral::UPDATE_CUT) {
        
        ReleaseCut cut;
        cut.setId(cmd->getIntParameter("id"));
        cut.setDatas(cmd->getStringParameter("description"), cmd->getIntParameter("position"), cmd->getIntParameter("size"));
        Query* query = cut.updateQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Cut updated");
            response->setDatas("<id>" + std::to_string(result->getLastInsertedId()) + "</id>");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Cut update failed");
        }
        delete query;
        delete result;
    }

    // -- DELETE
    else if (cmd->getType() == CommandCentral::DELETE_GROUP) {
        
        Group group;
        group.setId(cmd->getIntParameter("id"));
        Query* query = group.deleteQuery();
        ResultQuery *result = context->executeQuery(query);

        LinkParam param(LinkParam::CINEMA_GROUP);
        param.setDatas(-1, cmd->getIntParameter("id"));
        Query* queryLink = param.deleteFromDstQuery();
        ResultQuery *resultLink = context->executeQuery(queryLink);

        LinkParam param2(LinkParam::GROUP_RELEASE);
        param2.setDatas(cmd->getIntParameter("id"), -1);
        Query* queryLink2 = param2.deleteFromSrcQuery();
        ResultQuery *resultLink2 = context->executeQuery(queryLink2);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Group deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Group delete failed");
        }
        delete query;
        delete queryLink;
        delete queryLink2;
        delete result;
        delete resultLink;
        delete resultLink2;
    }
    else if (cmd->getType() == CommandCentral::DELETE_CINEMA) {

        int id_cinema = cmd->getIntParameter("id");

        Cinema cinema;
        cinema.setId(id_cinema);
        Query* query = cinema.deleteQuery();
        ResultQuery *result = context->executeQuery(query);

        LinkParam param(LinkParam::CINEMA_GROUP);
        param.setDatas(cmd->getIntParameter("id"), -1);
        Query* queryLink = param.deleteFromSrcQuery();
        ResultQuery *resultLink = context->executeQuery(queryLink);
        delete queryLink;
        delete resultLink;

        LinkParam param2(LinkParam::CINEMA_RELEASE);
        param2.setDatas(cmd->getIntParameter("id"), -1);
        Query* queryLink2 = param2.deleteFromSrcQuery();
        ResultQuery *resultLink2 = context->executeQuery(queryLink2);
        delete queryLink2;
        delete resultLink2;

        // -- delete servers of auditoriums
        Query* queryGetAuditorium = Auditorium::deleteQueryForCinema(&id_cinema);
        ResultQuery *resultGetAudi = context->executeQuery(queryGetAuditorium);
        if (resultGetAudi != nullptr && resultGetAudi->isValid()) {
            std::map<int, std::shared_ptr<Auditorium>>::iterator it;
            std::map<int, std::shared_ptr<Auditorium>> auditoriums = Auditorium::loadListFromResult(resultGetAudi);
            for (it = auditoriums.begin(); it != auditoriums.end(); it++) {
                int id_audi = it->second->getId();
                Query* queryDeleteServer = Server::deleteQueryForAuditorium(&id_audi);
                ResultQuery *resultDelete = context->executeQuery(queryDeleteServer);
                delete queryDeleteServer;
                delete resultDelete;
            }
        }
        delete queryGetAuditorium;
        delete resultGetAudi;

        Query* queryAuditorium = Auditorium::deleteQueryForCinema(&id_cinema);
        ResultQuery *resultAudi = context->executeQuery(queryAuditorium);
        delete queryAuditorium;
        delete resultAudi;

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Cinema deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Cinema delete failed");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::DELETE_AUDITORIUM) {

        int id_audi = cmd->getIntParameter("id");

        Auditorium audi;
        audi.setId(id_audi);
        Query* query = audi.deleteQuery();
        ResultQuery *result = context->executeQuery(query);

        Query* queryServer = Server::deleteQueryForAuditorium(&id_audi);
        ResultQuery *resultServer = context->executeQuery(queryServer);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Auditorium deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Auditorium delete failed");
        }
        delete query;
        delete result;
        delete queryServer;
        delete resultServer;
    }
    else if (cmd->getType() == CommandCentral::DELETE_SERVER) {

        Server server;
        server.setId(cmd->getIntParameter("id"));
        Query* query = server.deleteQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Server deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Server delete failed");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::DELETE_SCRIPT) {

        Script script;
        script.setId(cmd->getIntParameter("id"));
        Query* query = script.deleteQuery();
        ResultQuery *result = context->executeQuery(query);

        LinkParam param(LinkParam::SCRIPT_RELEASE);
        param.setDatas(cmd->getIntParameter("id"), -1);
        Query* queryLink = param.deleteFromSrcQuery();
        ResultQuery *resultLink = context->executeQuery(queryLink);
        delete queryLink;
        delete resultLink;

        LinkParam param2(LinkParam::CPL_SCRIPT);
        param2.setDatas(-1, cmd->getIntParameter("id"));
        Query* queryLink2 = param.deleteFromDstQuery();
        ResultQuery *resultLink2 = context->executeQuery(queryLink2);
        delete queryLink2;
        delete resultLink2;

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Script deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Script delete failed");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::DELETE_CPL) {

        Cpl cpl;
        cpl.setId(cmd->getIntParameter("id"));
        Query* query = cpl.deleteQuery();
        ResultQuery *result = context->executeQuery(query);

        LinkParam param(LinkParam::CPL_RELEASE);
        param.setDatas(cmd->getIntParameter("id"), -1);
        Query* queryLink = param.deleteFromSrcQuery();
        ResultQuery *resultLink = context->executeQuery(queryLink);
        delete queryLink;
        delete resultLink;

        LinkParam param2(LinkParam::CPL_SCRIPT);
        param2.setDatas(cmd->getIntParameter("id"), -1);
        Query* queryLink2 = param.deleteFromSrcQuery();
        ResultQuery *resultLink2 = context->executeQuery(queryLink2);
        delete queryLink2;
        delete resultLink2;

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Cpl deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Cpl delete failed");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::DELETE_RELEASE) {

        int id_release = cmd->getIntParameter("id");
        Release release;
        release.setId(id_release);

        Query* queryGetRelease = Release::getQueryForParent(&id_release);
        ResultQuery *resultGetRelease = context->executeQuery(queryGetRelease);
        std::map<int, std::shared_ptr<Release>> childs = Release::loadListFromResult(resultGetRelease);
        delete queryGetRelease;
        delete resultGetRelease;

        if (childs.size() == 0) {
            // -- delete release
            Query* query = release.deleteQuery();
            ResultQuery *result = context->executeQuery(query);

            // -- delete association script release
            LinkParam param(LinkParam::SCRIPT_RELEASE);
            param.setDatas(-1, id_release);
            Query* queryLink = param.deleteFromDstQuery();
            ResultQuery *resultLink = context->executeQuery(queryLink);
            delete queryLink;
            delete resultLink;

            // -- get link cpl to delete cpl related to release
            LinkParam param2(LinkParam::CPL_RELEASE);
            param2.setDatas(-1, id_release);
            Query* queryLinkCplRelease = param2.getQuerySrc(LinkParam::CPL_RELEASE, &id_release);
            ResultQuery *resultLinkCplRelease = context->executeQuery(queryLinkCplRelease);
            std::vector<std::shared_ptr<LinkParam>> cplsToRelease = LinkParam::loadListFromResult(resultLinkCplRelease, LinkParam::CPL_RELEASE);
            for (std::shared_ptr<LinkParam> link : cplsToRelease) {
                Cpl cpl;
                cpl.setId(link->getSrcId());
                Query* queryDelCpl = cpl.deleteQuery();
                ResultQuery *resultDelCpl = context->executeQuery(queryDelCpl);
                delete queryDelCpl;
                delete resultDelCpl;
            }
            delete queryLinkCplRelease;
            delete resultLinkCplRelease;
            Query* queryLink2 = param2.deleteFromDstQuery();
            ResultQuery *resultLink2 = context->executeQuery(queryLink2);
            delete queryLink2;
            delete resultLink2;

            LinkParam param3(LinkParam::CINEMA_RELEASE);
            param3.setDatas(-1, id_release);
            Query* queryLink3 = param3.deleteFromDstQuery();
            ResultQuery *resultLink3 = context->executeQuery(queryLink3);
            delete queryLink3;
            delete resultLink3;

            LinkParam param4(LinkParam::GROUP_RELEASE);
            param4.setDatas(-1, id_release);
            Query* queryLink4 = param4.deleteFromDstQuery();
            ResultQuery *resultLink4 = context->executeQuery(queryLink4);
            delete queryLink4;
            delete resultLink4;

            // -- delete cuts
            Query* queryDelCuts = ReleaseCut::deleteQueryForRelease(&id_release);
            ResultQuery *resultDelCuts = context->executeQuery(queryDelCuts);
            delete queryDelCuts;
            delete resultDelCuts;

            if (result != nullptr && result->isValid()) {
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Release deleted");
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Release delete failed");
            }
            delete query;
            delete result;
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Release is parent of another release !");
        }
        
    }
    else if (cmd->getType() == CommandCentral::DELETE_FEATURE) {

        int id_feature = cmd->getIntParameter("id");

        Feature feature;
        feature.setId(id_feature);
        Query* query = feature.deleteQuery();
        ResultQuery *result = context->executeQuery(query);

        Query* queryReleases = Release::getQueryForFeature(&id_feature);
        ResultQuery *resultReleases = context->executeQuery(queryReleases);
        std::map<int, std::shared_ptr<Release>> releases = Release::loadListFromResult(resultReleases);
        std::map<int, std::shared_ptr<Release>>::iterator itRelease;
        delete queryReleases;
        delete resultReleases;

        for (itRelease = releases.begin() ; itRelease != releases.end(); itRelease++) {
            int id_release = itRelease->first;

            // -- delete release
            Release release;
            release.setId(id_release);
            Query* queryDelRelease = release.deleteQuery();
            ResultQuery *resultDelRelease = context->executeQuery(queryDelRelease);
            delete queryDelRelease;
            delete resultDelRelease;

            // -- delete association script release
            LinkParam param(LinkParam::SCRIPT_RELEASE);
            param.setDatas(-1, id_release);
            Query* queryLink = param.deleteFromDstQuery();
            ResultQuery *resultLink = context->executeQuery(queryLink);
            delete queryLink;
            delete resultLink;

            // -- get link cpl to delete cpl related to release
            LinkParam param2(LinkParam::CPL_RELEASE);
            param2.setDatas(-1, id_release);
            Query* queryLinkCplRelease = param2.getQuerySrc(LinkParam::CPL_RELEASE, &id_release);
            ResultQuery *resultLinkCplRelease = context->executeQuery(queryLinkCplRelease);
            std::vector<std::shared_ptr<LinkParam>> cplsToRelease = LinkParam::loadListFromResult(resultLinkCplRelease, LinkParam::CPL_RELEASE);
            for (std::shared_ptr<LinkParam> link : cplsToRelease) {
                Cpl cpl;
                cpl.setId(link->getSrcId());
                Query* queryDelCpl = cpl.deleteQuery();
                ResultQuery *resultDelCpl = context->executeQuery(queryDelCpl);
                delete queryDelCpl;
                delete resultDelCpl;
            }
            delete queryLinkCplRelease;
            delete resultLinkCplRelease;
            Query* queryLink2 = param2.deleteFromDstQuery();
            ResultQuery *resultLink2 = context->executeQuery(queryLink2);
            delete queryLink2;
            delete resultLink2;

            LinkParam param3(LinkParam::CINEMA_RELEASE);
            param3.setDatas(-1, id_release);
            Query* queryLink3 = param3.deleteFromDstQuery();
            ResultQuery *resultLink3 = context->executeQuery(queryLink3);
            delete queryLink3;
            delete resultLink3;

            LinkParam param4(LinkParam::GROUP_RELEASE);
            param4.setDatas(-1, id_release);
            Query* queryLink4 = param4.deleteFromDstQuery();
            ResultQuery *resultLink4 = context->executeQuery(queryLink4);
            delete queryLink4;
            delete resultLink4;

            // -- delete cuts
            Query* queryDelCuts = ReleaseCut::deleteQueryForRelease(&id_release);
            ResultQuery *resultDelCuts = context->executeQuery(queryDelCuts);
            delete queryDelCuts;
            delete resultDelCuts;
        }

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Feature deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Feature delete failed");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::DELETE_CUT) {
        
        ReleaseCut cut;
        cut.setId(cmd->getIntParameter("id"));
        Query* query = cut.deleteQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Cut deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Cut delete failed");
        }
        delete query;
        delete result;
    }

    // -- ASSOCIATIONS GROUP/CINEMA
    else if (cmd->getType() == CommandCentral::ADD_CINEMA_TO_GROUP) {

        LinkParam param(LinkParam::CINEMA_GROUP);
        param.setDatas(cmd->getIntParameter("id_cinema"), cmd->getIntParameter("id_group"));
        Query* query = param.createQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Link cinema to group created");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Link cinema to group create failed");
            response->setDatas("<error><code>" + std::to_string(result->getErrorCode()) + "</code><message>" + result->getErrorMessage() + "</message></error>");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::REMOVE_CINEMA_TO_GROUP) {

        LinkParam param(LinkParam::CINEMA_GROUP);
        param.setDatas(cmd->getIntParameter("id_cinema"), cmd->getIntParameter("id_group"));
        Query* query = param.deletePreciseQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Link cinema to group deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Link cinema to group delete failed");
            response->setDatas("<error><code>" + std::to_string(result->getErrorCode()) + "</code><message>" + result->getErrorMessage() + "</message></error>");
        }
        delete query;
        delete result;
    }
    
    // -- ASSOCIATIONS RELEASE
    else if (cmd->getType() == CommandCentral::ADD_GROUP_TO_RELEASE) {

        LinkParam param(LinkParam::GROUP_RELEASE);
        param.setDatas(cmd->getIntParameter("id_group"), cmd->getIntParameter("id_release"));
        Query* query = param.createQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Link group to release created");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Link group to release create failed");
        }
        delete query;
    }
    else if (cmd->getType() == CommandCentral::REMOVE_GROUP_TO_RELEASE) {

        LinkParam param(LinkParam::GROUP_RELEASE);
        param.setDatas(cmd->getIntParameter("id_group"), cmd->getIntParameter("id_release"));
        Query* query = param.deletePreciseQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Link group to release deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Link group to release delete failed");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::ADD_CINEMA_TO_RELEASE) {
        
        LinkParam param(LinkParam::CINEMA_RELEASE);
        param.setDatas(cmd->getIntParameter("id_cinema"), cmd->getIntParameter("id_release"));
        Query* query = param.createQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Link cinema to release created");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Link cinema to release create failed");
        }
        delete query;
    }
    else if (cmd->getType() == CommandCentral::REMOVE_CINEMA_TO_RELEASE) {

        LinkParam param(LinkParam::CINEMA_RELEASE);
        param.setDatas(cmd->getIntParameter("id_cinema"), cmd->getIntParameter("id_release"));
        Query* query = param.deletePreciseQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Link cinema to release deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Link cinema to release delete failed");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::ADD_CPL_TO_RELEASE) {

        LinkParam param(LinkParam::CPL_RELEASE);
        param.setDatas(cmd->getIntParameter("id_cpl"), cmd->getIntParameter("id_release"));
        Query* query = param.createQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Link cpl to release created");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Link cpl to release create failed");
        }
        delete query;
    }
    else if (cmd->getType() == CommandCentral::REMOVE_CPL_TO_RELEASE) {

        LinkParam param(LinkParam::CPL_RELEASE);
        param.setDatas(cmd->getIntParameter("id_cpl"), cmd->getIntParameter("id_release"));
        Query* query = param.deletePreciseQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Link cpl to release deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Link cpl to release delete failed");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::ADD_CPL_TO_SCRIPT) {

        LinkParam param(LinkParam::CPL_SCRIPT);
        param.setDatas(cmd->getIntParameter("id_cpl"), cmd->getIntParameter("id_script"));
        Query* query = param.createQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Link cpl to script created");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Link cpl to script create failed");
        }
        delete query;
    }
    else if (cmd->getType() == CommandCentral::REMOVE_CPL_TO_SCRIPT) {

        LinkParam param(LinkParam::CPL_SCRIPT);
        param.setDatas(cmd->getIntParameter("id_cpl"), cmd->getIntParameter("id_script"));
        Query* query = param.deletePreciseQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Link cpl to script deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Link cpl to script delete failed");
        }
        delete query;
        delete result;
    }
    else if (cmd->getType() == CommandCentral::ADD_SCRIPT_TO_RELEASE) {

        LinkParam param(LinkParam::SCRIPT_RELEASE);
        param.setDatas(cmd->getIntParameter("id_script"), cmd->getIntParameter("id_release"));
        Query* query = param.createQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Link script to release created");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Link script to release create failed");
        }
        delete query;
    }
    else if (cmd->getType() == CommandCentral::REMOVE_SCRIPT_TO_RELEASE) {
        
        LinkParam param(LinkParam::SCRIPT_RELEASE);
        param.setDatas(cmd->getIntParameter("id_script"), cmd->getIntParameter("id_release"));
        Query* query = param.deletePreciseQuery();
        ResultQuery *result = context->executeQuery(query);

        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Link script to release deleted");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Link script to release delete failed");
        }
        delete query;
        delete result;
    }

    // -- STATE OF DOWNLOADED SCRIPTS BY SERVER FOR A RELEASE
    else if (cmd->getType() == CommandCentral::GET_STATE_AUDITORIUM) {

        std::string xml = "<auditoriums>";
        std::map<int, std::shared_ptr<StateServer>> states = context->getStateAuditoriums();
        std::map<int, std::shared_ptr<StateServer>>::iterator it;
        for (it = states.begin(); it != states.end(); it++) {
            xml += it->second->toXmlString();
        } 
        xml += "</auditoriums>";
        response->setStatus(CommandCentralResponse::OK);
        response->setComments("GET_STATE_AUDITORIUM success !");
        response->setDatas(xml);
    }
    else if (cmd->getType() == CommandCentral::GET_STATE_SERVER) {
        std::string xml = "<servers>";
        std::map<int, std::shared_ptr<StateServer>> states = context->getStateServers();
        std::map<int, std::shared_ptr<StateServer>>::iterator it;
        for (it = states.begin(); it != states.end(); it++) {
            xml += it->second->toXmlString();
        } 
        xml += "</servers>";
        response->setStatus(CommandCentralResponse::OK);
        response->setComments("GET_STATE_SERVER success !");
        response->setDatas(xml);
    }
    else if (cmd->getType() == CommandCentral::GET_SCRIPTS_FOR_RELEASE) {
        
        int idRelease = cmd->getIntParameter("id_release");
        std::map<int, std::shared_ptr<ScriptState>> states = context->getScriptsInfos();
        std::map<int, std::shared_ptr<ScriptState>>::iterator it = states.find(idRelease);

        if (it == states.end()) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("GET_SCRIPTS_FOR_RELEASE failed !");
        }
        else {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("GET_SCRIPTS_FOR_RELEASE success !");
            response->setDatas(it->second->toXmlString());
        }
    }
    else if (cmd->getType() == CommandCentral::GET_RESULT_SYNCHRO) {

        std::string uuid = cmd->getStringParameter("uuid");
        std::map<std::string, std::string> outputs(context->getOutputsSynchro());
        std::map<std::string, std::string>::iterator it = outputs.find(uuid);
        if (it == outputs.end()) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("No outputs for uuid found !");
        }
        else {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Outputs found !");
            response->setDatas(it->second);
        }
    }
    
    // -- GET LIST OF SCRIPTS WITH SYNC FILE NEEDED FOR A GIVEN CINEMA / TYPE OF SERVER
    else if (cmd->getType() == CommandCentral::GET_SCRIPTS_FOR_SERVER) {
        
        std::string nameCinema = cmd->getStringParameter("name_cinema");
        bool isFeatureServer = cmd->getBoolParameter("isFeatureServer");
        std::map<int, std::shared_ptr<Cinema>> cinemas = context->prepareCinemas();

        int id_cinema = -1;
        std::map<int, std::shared_ptr<Cinema>>::iterator itCinema;
        for (itCinema = cinemas.begin(); itCinema != cinemas.end(); itCinema++) {
            if (itCinema->second->getName() == nameCinema) {
                id_cinema = itCinema->second->getId();
                break;
            }
        }

        if (id_cinema == -1) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Cinema not found with name : " + nameCinema);
        }
        else {

            std::string datas = "<scripts>";

            // -- for all releases
            std::map<int, std::shared_ptr<Release>> releases = context->prepareRelease();
            std::map<int, std::shared_ptr<Release>>::iterator itRelease;
            for (itRelease = releases.begin(); itRelease != releases.end(); itRelease++) {

                std::shared_ptr<Release> release = itRelease->second;
                Poco::Logger::get("ContextCentralThread").information("Treat release " + release->getName(), __FILE__, __LINE__);

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

                Poco::Logger::get("ContextCentralThread").information("Check all associated cinemas !", __FILE__, __LINE__);
                std::vector<int> idCinemas = context->associateRelatedCinema(release);

                bool hasRight = false;
                for (int id : idCinemas) {
                    if (id == id_cinema) {
                        hasRight = true;
                        break;
                    }
                }

                if (!hasRight) {
                    continue;
                }

                if (isFeatureServer) {
                    // -- top of release to get scripts
                    std::shared_ptr<Release> refRelease = release;
                    while (refRelease->getParentRelease() != nullptr) {
                        refRelease = refRelease->getParentRelease();
                    }

                    if (refRelease->getFeatureScript() != nullptr) {
                        std::shared_ptr<Script> script = refRelease->getFeatureScript();
                        datas += "<feature name=\"" + script->getName() + "\">";
                        Poco::File pathScript(script->getPath());
                        std::vector<Poco::File> files;
                        pathScript.list(files);
                        for (Poco::File file : files) {
                            datas += "<file path=\"" + file.path() + "\" />";
                        }

                        for (std::shared_ptr<Cpl> cpl : release->getFeatureCpls()) {
                            if (!cpl->getSyncPath().empty()) {
                                datas += "<file path=\"" + cpl->getSyncPath() + "\" />";
                            }
                        }
                        datas += "</feature>";
                    }

                    if (refRelease->getLoopScript() != nullptr) {
                        std::shared_ptr<Script> script = refRelease->getLoopScript();
                        datas += "<loop name=\"" + script->getName() + "\">";
                        Poco::File pathScript(script->getPath());
                        std::vector<Poco::File> files;
                        pathScript.list(files);
                        for (Poco::File file : files) {
                            datas += "<file path=\"" + file.path() + "\" />";
                        }

                        datas += "</loop>";
                    }
                }
                else {

                    // -- top of release to get scripts
                    std::shared_ptr<Release> refRelease = release;
                    while (refRelease->getParentRelease() != nullptr) {
                        refRelease = refRelease->getParentRelease();
                    }

                    if (refRelease->getSasScript() != nullptr) {
                        std::shared_ptr<Script> script = refRelease->getSasScript();
                        datas += "<sas name=\"" + script->getName() + "\">";
                        Poco::File pathScript(script->getPath());
                        std::vector<Poco::File> files;
                        pathScript.list(files);
                        for (Poco::File file : files) {
                            datas += "<file path=\"" + file.path() + "\" />";
                        }

                        datas += "</sas>";
                    }
                }
            }
        
            datas += "</scripts>";

            response->setStatus(CommandCentralResponse::OK);
            response->setComments("GET_SCRIPTS_FOR_SERVER success !");
            response->setDatas(datas);
        }
    }
    // -- Getters State Machine
    else if (cmd->getType() == CommandCentral::GET_CONTENT) {

        int contentId = cmd->getIntParameter("id_content");
        if (contentId == -1) {
            try {
                std::string contentsXml = _boundaryManager.GetAllContentsAsXml();
                response->setDatas(contentsXml);
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Contents retrieved successfully.");
            }
            catch(std::exception e) {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Failed to get contents");
                response->setDatas("<error><code>100</code><message>" + std::string(e.what())+ "</message></error>");
                Poco::Logger::get("ContextThread").error("Error while calling BoundaryManager::GetAllContentsAsXml() :" + std::string(e.what()), __FILE__, __LINE__);
            
            }
        }
        else {
            try {
                std::string contentXml = _boundaryManager.GetContentAsXml(contentId);
                response->setDatas(contentXml);
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Content retrieved successfully.");
            }
            catch(std::exception e) {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Failed to get content");
                response->setDatas("<error><code>101</code><message>" + std::string(e.what())+ "</message></error>");
                Poco::Logger::get("ContextThread").error("Error while calling BoundaryManager::GetContentAsXml() :" + std::string(e.what()), __FILE__, __LINE__);
            
            }
        }
        /* Content* content = new Content();
        if (cmdId == -1) {
            contentRepo->Read(content);
            Query* query = contentRepo->GetQuery();
            ResultQuery *result = this->_dbConnection->ExecuteQuery(query);
            
            //TODO: Statemachine
            //content->SetStateMachine(StateMachineManager::GetInstance()->CreateStateMachine(*content->GetContentId(), this->_dbConnection));

            if (result != nullptr && result->isValid()) {
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Contents get success");
                std::string datas = "<contents>";
                for (int i = 0; i < result->getNbRows(); i++) {
                    content->SetContentId(*result->getIntValue(i, "id_content"));
                    content->SetDatas(*result->getStringValue(i, "title"));
                    datas += content->toXmlString(false);
                }
                datas += "</contents>";
                response->setDatas(datas);
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Contents get failed");
                response->setDatas("<error><code>" + std::to_string(result->getErrorCode()) + "</code><message>" + result->getErrorMessage() + "</message></error>");
            }
            delete query;
            delete result;
        }
        else {
            content->SetContentId(cmdId);
            contentRepo->Read(content);
            Query* query = contentRepo->GetQuery();
            ResultQuery *result = this->_dbConnection->ExecuteQuery(query);
            
            if (result != nullptr && result->isValid()) {
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Content get success");
                content->SetContentId(*result->getIntValue(0, "id_content"));
                content->SetDatas(*result->getStringValue(0, "title"));
                response->setDatas(content->toXmlString(true));
            }
            else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Content get failed");
                response->setDatas("<error><code>" + std::to_string(result->getErrorCode()) + "</code><message>" + result->getErrorMessage() + "</message></error>");
            }
            delete query;
            delete result;
        } */

        //delete contentRepo;
        //delete content;
    } 
    else if (cmd->getType() == CommandCentral::GET_RELEASES_CONTENT) {
        int contentId = cmd->getIntParameter("id_content");
        int typeId = cmd->getIntParameter("id_type");
        int localisationId = cmd->getIntParameter("id_localisation");
        if (typeId == -1 || localisationId == -1) {
            try {
                std::string releasesXml = _boundaryManager.GetContentReleasesAsXml(contentId);
                response->setDatas(releasesXml);
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Releases retrieved successfully.");
            }
            catch(std::exception e) {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Failed to get releases - 1");
                response->setDatas("<error><code>100</code><message>" + std::string(e.what())+ "</message></error>");
                Poco::Logger::get("ContextThread").error("Error while calling BoundaryManager::GetContentReleasesAsXml() :" + std::string(e.what()), __FILE__, __LINE__);
            }
        }else {
            try {
                std::string releasesXml = _boundaryManager.GetContentReleasesAsXml(contentId,typeId,localisationId);
                response->setDatas(releasesXml);
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Releases retrieved successfully.");
            }
            catch(std::exception e) {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Failed to get releases - 2");
                response->setDatas("<error><code>100</code><message>" + std::string(e.what())+ "</message></error>");
                Poco::Logger::get("ContextThread").error("Error while calling BoundaryManager::GetContentReleasesAsXml() :" + std::string(e.what()), __FILE__, __LINE__);
            }
        }
        //MySQLReleaseRepo* releaseRepo = new MySQLReleaseRepo();
        //int cmdId = cmd->getIntParameter("id_content");
        //int typeId = cmd->getIntParameter("id_type");
        //int LocalisationId = cmd->getIntParameter("id_localisation");
        //if (typeId == -1 || LocalisationId == -1) {
        //    Releases* release = new Releases();
        //    release->SetReleaseId(cmdId, TypeMovie::UNKNOW_TYPE, LocalisationMovie::UNKNOW_LOCALISATION);
        //    releaseRepo->Read(release);
        //    Query* query = releaseRepo->GetQuery();
        //    ResultQuery *result = this->_dbConnection->ExecuteQuery(query);
        //
        //    if (result != nullptr && result->isValid()) {
        //        response->setStatus(CommandCentralResponse::OK);
        //        response->setComments("Releases get success");
        //        std::string datas = "<releases>";
        //        for (int i = 0; i < result->getNbRows(); i++) {
        //            release->SetReleaseId(*result->getIntValue(i, "id_content"), static_cast<TypeMovie>(*result->getIntValue(i, "id_type")),  static_cast<LocalisationMovie>(*result->getIntValue(i, "id_localisation")));
        //            release->SetReleaseInfos(*result->getStringValue(i, "release_cpl_ref_path"));
        //            datas += release->toXmlString(false);
        //        }
        //        datas += "</releases>";
        //        response->setDatas(datas);
        //    }
        //    else {
        //        response->setStatus(CommandCentralResponse::KO);
        //        response->setComments("Releases get failed");
        //        response->setDatas("<error><code>" + std::to_string(result->getErrorCode()) + "</code><message>" + result->getErrorMessage() + "</message></error>");
        //    }
        //    delete query;
        //    delete result;
        //    delete release; 
        //}
        //else {
        //    Releases* release = new Releases(cmdId,cmd->getIntParameter("id_type"), cmd->getIntParameter("id_localisation"));
        //    releaseRepo->Read(release);
        //    Query* queryRelease = releaseRepo->GetQuery();
        //    ResultQuery *resultRelease = this->_dbConnection->ExecuteQuery(queryRelease);
        //
        //    MySQLCISRepo *cisRepo = new MySQLCISRepo();
        //    CIS* cis = new CIS();
        //    cis->SetCISId(cmdId,cmd->getIntParameter("id_type"), cmd->getIntParameter("id_localisation"));
        //    cisRepo->Read(cis);
        //    Query* queryCIS = cisRepo->GetQuery();
        //    ResultQuery *resultCIS = this->_dbConnection->ExecuteQuery(queryCIS);
//
        //    if (resultRelease != nullptr && resultRelease->isValid() && resultCIS != nullptr && resultCIS->isValid()) {
        //        response->setStatus(CommandCentralResponse::OK);
        //        response->setComments("Release get success");
        //        std::string datas = "<releases>";
        //        release->SetReleaseId(*resultRelease->getIntValue(0, "id_content"), static_cast<TypeMovie>(*resultRelease->getIntValue(0, "id_type")),  static_cast<LocalisationMovie>(*resultRelease->getIntValue(0, "id_localisation")));
        //        release->SetReleaseInfos(*resultRelease->getStringValue(0, "release_cpl_ref_path"));
        //        release->UploadCIS(*resultCIS->getStringValue(0, "release_cis_path"));
        //        datas += release->toXmlString(true);
        //        datas += "</releases>";
        //        response->setDatas(datas);
        //    }
        //    delete queryRelease;
        //    delete resultRelease;
        //    delete queryCIS;
        //    delete resultCIS;
        //    delete release;
        //}
        //delete releaseRepo;
    }
    else if (cmd->getType() == CommandCentral::GET_GROUPS_FILTER) {

        int groupId = cmd->getIntParameter("id_group");
        if (groupId == -1) {
            try {
                std::string groupsXml = _boundaryManager.GetGroupsAsXml();
                response->setDatas(groupsXml);
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Groups retreived successfully.");
            }
            catch(std::exception e) {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Failed to get groups");
                response->setDatas("<error><code>100</code><message>" + std::string(e.what())+ "</message></error>");
                Poco::Logger::get("ContextThread").error("Error while calling BoundaryManager::GetGroupsAsXml() :" + std::string(e.what()), __FILE__, __LINE__);
            }
        } else {  
            try {
                std::string groupXml = _boundaryManager.GetGroupAsXml(groupId);
                response->setDatas(groupXml);
                response->setStatus(CommandCentralResponse::OK);
                response->setComments("Group retreived successfully.");
            }
            catch(std::exception e) {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Failed to get group");
                response->setDatas("<error><code>100</code><message>" + std::string(e.what())+ "</message></error>");
                Poco::Logger::get("ContextThread").error("Error while calling BoundaryManager::GetGroupAsXml() :" + std::string(e.what()), __FILE__, __LINE__);
            }  
        }
       // if (cmdId == -1) {
       //     Query* query =  new Query(Query::SELECT, "ice", "groups");
       //     query->addParameter("id_group", nullptr, "int");
       //     query->addParameter("id_group_1", nullptr, "int");
       //     query->addParameter("name", nullptr, "string");
       //     ResultQuery* result = this->_dbConnection->ExecuteQuery(query);
       //     if (result->isValid()) {
       //         response->setStatus(CommandCentralResponse::OK);
       //         response->setComments("Groups get success");
       //         std::string datas = "<groups>";
       //         for (int i = 0; i < result->getNbRows(); i++) {
       //             std::string GroupParent;
       //             result->getIntValue(i, "id_group_1") == nullptr ? GroupParent = "" : GroupParent = "\" id_group_1=\"" + std::to_string(*result->getIntValue(i, "id_group_1"));      
       //             datas += "<group id_group=\"" + std::to_string(*result->getIntValue(i, "id_group")) + GroupParent + "\" name=\"" + *result->getStringValue(i, "name") + "\" />";
       //         }
       //         datas += "</groups>";
       //         response->setDatas(datas);
       //     }
       //     else {
       //         response->setStatus(CommandCentralResponse::KO);
       //         response->setComments("Groups get failed");
       //         response->setDatas("<error><code>" + std::to_string(result->getErrorCode()) + "</code><message>" + result->getErrorMessage() + "</message></error>");
       //     }
       // }
       // else {
       //     Query* query =  new Query(Query::SELECT, "ice", "groups");
       //     query->addParameter("id_group", nullptr, "int");
       //     query->addParameter("id_group_1", nullptr, "int");
       //     query->addParameter("name", nullptr, "string");
       //     query->addWhereParameter("id_group",&cmdId , "int");
       //     ResultQuery* result = this->_dbConnection->ExecuteQuery(query);
       //     if (result->isValid()) {
       //         response->setStatus(CommandCentralResponse::OK);
       //         response->setComments("Groups get success");
       //         std::string datas = "<groups>";
       //         for (int i = 0; i < result->getNbRows(); i++) {
       //             std::string GroupParent;
       //             result->getIntValue(i, "id_group_1") == nullptr ? GroupParent = "" : GroupParent = "\" id_group_1=\"" + std::to_string(*result->getIntValue(i, "id_group_1"));      
       //             datas += "<group id_group=\"" + std::to_string(*result->getIntValue(i, "id_group")) + GroupParent + "\" name=\"" + *result->getStringValue(i, "name") + "\" />";
       //         }
       //         datas += "</groups>";
       //         response->setDatas(datas);
       //     }
       //     else {
       //         response->setStatus(CommandCentralResponse::KO);
       //         response->setComments("Groups get failed");
       //         response->setDatas("<error><code>" + std::to_string(result->getErrorCode()) + "</code><message>" + result->getErrorMessage() + "</message></error>");
       //     }
       // }
    }
    else if (cmd->getType() == CommandCentral::GET_SITES) {
        int groupId = cmd->getIntParameter("id_group");
        try {
            std::string sitesXml = _boundaryManager.GetSitesAsXml(groupId);
            response->setDatas(sitesXml);
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Sites retreived successfully.");
        }
        catch(std::exception e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Failed to get sites");
            response->setDatas("<error><code>100</code><message>" + std::string(e.what())+ "</message></error>");
            Poco::Logger::get("ContextThread").error("Error while calling BoundaryManager::GetSitesAsXml() :" + std::string(e.what()), __FILE__, __LINE__);
        }
        //Query* query =  new Query(Query::SELECT, "ice", "site");
        //query->addParameter("id_site", nullptr, "int");
        //query->addParameter("id_group", nullptr, "int");
        //query->addParameter("id_connection", nullptr, "int");
        //query->addParameter("name", nullptr, "string");
        //query->addWhereParameter("id_group", &groupId, "int");
        //ResultQuery* result = this->_dbConnection->ExecuteQuery(query);
        //if (result->isValid()) {
        //    response->setStatus(CommandCentralResponse::OK);
        //    response->setComments("Sites get success");
        //    std::string datas = "<sites>";
        //    for (int i = 0; i < result->getNbRows(); i++) {
        //        datas += "<site id_site=\"" + std::to_string(*result->getIntValue(i, "id_site")) + "\" id_group=\"" + std::to_string(*result->getIntValue(i, "id_group")) + "\" id_connection=\"" + std::to_string(*result->getIntValue(i, "id_connection")) + "\" name=\"" + *result->getStringValue(i, "name") + "\" />";
        //    }
        //    datas += "</sites>";
        //    response->setDatas(datas);
        //}
        //else {
        //    response->setStatus(CommandCentralResponse::KO);
        //    response->setComments("Sites get failed");
        //    response->setDatas("<error><code>" + std::to_string(result->getErrorCode()) + "</code><message>" + result->getErrorMessage() + "</message></error>");
        //}
    }
    else if (cmd->getType() == CommandCentral::GET_CPLS_SITE) {
        int siteId = cmd->getIntParameter("id_site");

        try {
            std::string cplsXml = _boundaryManager.GetSiteCplsAsXml(siteId);
            response->setDatas(cplsXml);
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Cpls retreived successfully.");
        }
        catch(std::exception e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Failed to get cpls");
            response->setDatas("<error><code>100</code><message>" + std::string(e.what())+ "</message></error>");
            Poco::Logger::get("ContextThread").error("Error while calling BoundaryManager::GetSiteCplsAsXml() :" + std::string(e.what()), __FILE__, __LINE__);
        }

        //MySQLCPLRepo* cplRepo = new MySQLCPLRepo();
        //int siteId = cmd->getIntParameter("id_site");
        //Query* queryIceServerPairConfig = new Query(Query::SELECT, "ice", "server_pair_configuration");
        //queryIceServerPairConfig->addParameter("id_serv_pair_config", nullptr, "int");
        //queryIceServerPairConfig->addParameter("id_site", nullptr, "int");
        //queryIceServerPairConfig->addWhereParameter("id_site", &siteId, "int");
        //ResultQuery* resultIceServerPairConfig = this->_dbConnection->ExecuteQuery(queryIceServerPairConfig);
        //CPLRelease* cpl = new CPLRelease();
        //cpl->SetCPLId(-1, -1, -1, -1);
        //cplRepo->Read(cpl);
        //Query* queryCpl = cplRepo->GetQuery();
        //ResultQuery *resultCPL = this->_dbConnection->ExecuteQuery(queryCpl);
        //if (resultCPL != nullptr && resultCPL->isValid() && resultIceServerPairConfig != nullptr && resultIceServerPairConfig->isValid() && resultIceServerPairConfig->getNbRows() > 0) {
        //    response->setStatus(CommandCentralResponse::OK);
        //    response->setComments("CPLs get success");
        //    std::string datas = "<cpls>";
        //    for (int i = 0; i < resultCPL->getNbRows(); i++) {
        //        for (int j = 0; j < resultIceServerPairConfig->getNbRows(); j++) {
        //            if (*resultCPL->getIntValue(i, "id_serv_pair_config") == *resultIceServerPairConfig->getIntValue(j, "id_serv_pair_config")){
        //                cpl->SetCPLId(*resultCPL->getIntValue(i, "id_serv_pair_config"), *resultCPL->getIntValue(i, "id_content"), *resultCPL->getIntValue(i, "id_type"), *resultCPL->getIntValue(i, "id_localisation"));
        //                cpl->SetDatas(*resultCPL->getStringValue(i, "uuid"), *resultCPL->getStringValue(i, "name"));
        //                cpl->SetCplInfos(*resultCPL->getStringValue(i, "path_cpl"));
        //                datas += cpl->toXmlString();
        //            }
        //        }
        //    }
        //    datas += "</cpls>";
        //    response->setDatas(datas);
        //}
        //else {
        //    response->setStatus(CommandCentralResponse::KO);
        //    response->setComments("CPLs get failed");
        //    response->setDatas("<error><code>" + std::to_string(resultCPL->getErrorCode()) + "</code><message>" + resultCPL->getErrorMessage() + "</message></error>");
        //}
        //delete cplRepo;
        //delete cpl;
    }
    else if (cmd->getType() == CommandCentral::UPDATE_RELEASE_CONTENT) {
        MySQLReleaseRepo* releaseRepo = new MySQLReleaseRepo();
        int contentId = cmd->getIntParameter("id_content");
        int typeId = cmd->getIntParameter("id_type");
        int LocalisationId = cmd->getIntParameter("id_localisation");
        COD_Releases* release = new COD_Releases(contentId, typeId, LocalisationId);
        std::string path = cmd->getStringParameter("release_cpl_ref_path");
        release->SetReleaseInfos(path);
        releaseRepo->Update(release);
        Query* query = releaseRepo->GetQuery();
        ResultQuery *result = this->_dbConnection->ExecuteQuery(query);
        if (result != nullptr && result->isValid()) {
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Release update success");
        }
        else {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Release update failed");
            response->setDatas("<error><code>" + std::to_string(result->getErrorCode()) + "</code><message>" + result->getErrorMessage() + "</message></error>");
        }
    }
    else if (cmd->getType() == CommandCentral::GET_RELEASE_CPLS) {
        int contentId = cmd->getIntParameter("id_content");
        int typeId = cmd->getIntParameter("id_type");
        int localisationId = cmd->getIntParameter("id_localisation");
        try {
            std::string datas = BoundaryManager::GetInstance().GetReleaseCplsAsXml(contentId, typeId, localisationId);
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("CPLs get success");
            response->setDatas(datas);
        } catch (const std::exception& e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("CPLs get failed");
            response->setDatas(std::string("<error><message>") + e.what() + "</message></error>");
        }
    }
    else if (cmd->getType() == CommandCentral::GET_RELEASE_SYNCS) {
        int contentId = cmd->getIntParameter("id_content");
        int typeId = cmd->getIntParameter("id_type");
        int localisationId = cmd->getIntParameter("id_localisation");
        
        try {
            std::string syncsXml = _boundaryManager.GetReleaseSyncsAsXml(contentId, typeId, localisationId);
            response->setDatas(syncsXml);
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Syncs get success");
            
            Poco::Logger::get("ContextCentralThread").information(
                "Syncs retrieved for release " + std::to_string(contentId) + 
                "_" + std::to_string(typeId) + "_" + std::to_string(localisationId), __FILE__, __LINE__);
        }
        catch (const std::exception& e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Syncs get failed: " + std::string(e.what()));
            response->setDatas("<e><message>" + std::string(e.what()) + "</message></e>");
            
            Poco::Logger::get("ContextCentralThread").error(
                "Error getting syncs: " + std::string(e.what()), __FILE__, __LINE__);
        }
    }
    else if (cmd->getType() == CommandCentral::GET_RELEASE_SYNCLOOPS) {
        response->setStatus(CommandCentralResponse::KO);
        response->setComments("Commande GET_RELEASE_SYNCLOOPS obsolète : gestion SyncLoop supprimée.");
        response->setDatas("<e><message>Commande GET_RELEASE_SYNCLOOPS obsolète</message></e>");
    }
    else if (cmd->getType() == CommandCentral::GET_SERVER_PAIR) {
        int serverPairId = cmd->getIntParameter("id_serv_pair_config");
        
        try {
            std::string serverPairsXml;
            
            if (serverPairId != -1) {
                serverPairsXml = _boundaryManager.GetServerPairsAsXml(serverPairId);
                serverPairsXml = "<servPairs>" + serverPairsXml + "</servPairs>";
            } else {
                serverPairsXml = _boundaryManager.GetServerPairsAsXml();
            }
            
            response->setDatas(serverPairsXml);
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Server pair get success");
            
            Poco::Logger::get("ContextCentralThread").information(
                "Server pairs retrieved successfully", __FILE__, __LINE__);
        }
        catch (const std::exception& e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Server pair get failed: " + std::string(e.what()));
            response->setDatas("<e><message>" + std::string(e.what()) + "</message></e>");
            
            Poco::Logger::get("ContextCentralThread").error(
                "Error getting server pairs: " + std::string(e.what()), __FILE__, __LINE__);
        }
    }
    else if(cmd->getType() == CommandCentral::CREATE_CONTENT)
    {
        try {
            std::unique_ptr<COB_Content> createdContent = _boundaryManager.CreateContent(cmd->getStringParameter("contentTitle"));
            
            if (createdContent) {
                std::string contentXml = static_cast<std::string>(*createdContent);
                response->setDatas(contentXml);
            response->setStatus(CommandCentralResponse::OK);
                response->setComments("Content created successfully");
            } else {
                response->setStatus(CommandCentralResponse::KO);
                response->setComments("Content creation failed");
                response->setDatas("<error><code>101</code><message>Failed to create content</message></error>");
            }
        }  
        catch(std::exception& e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Content creation failed");
            response->setDatas("<error><code>102</code><message>" + std::string(e.what())+ "</message></error>");
            Poco::Logger::get("ContextThread").error("Error while calling BoundaryManager::CreateContent() :" + std::string(e.what()), __FILE__, __LINE__);
        }

    }
    else if (cmd->getType() == CommandCentral::GET_LOCALISATIONS) {
        try {
            std::string localisationsXml = _boundaryManager.GetLocalisationsAsXml();
            response->setDatas(localisationsXml);
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Localisations retrieved successfully.");
        }
        catch(std::exception e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Failed to get localisations");
            response->setDatas("<error><code>100</code><message>" + std::string(e.what())+ "</message></error>");
            Poco::Logger::get("ContextThread").error("Error while calling BoundaryManager::GetLocalisationsAsXml() :" + std::string(e.what()), __FILE__, __LINE__);
        }
    }
    else if (cmd->getType() == CommandCentral::GET_TYPES) {
        try {
            std::string typesXml = _boundaryManager.GetTypesAsXml();
            response->setDatas(typesXml);
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Types retrieved successfully.");
        }
        catch(std::exception e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Failed to get types");
            response->setDatas("<error><code>100</code><message>" + std::string(e.what())+ "</message></error>");
            Poco::Logger::get("ContextThread").error("Error while calling BoundaryManager::GetTypesAsXml() :" + std::string(e.what()), __FILE__, __LINE__);
        }
    }
    else if (cmd->getType() == CommandCentral::CREATE_RELEASE) {
        int contentId = cmd->getIntParameter("id_content");
        int typeId = cmd->getIntParameter("id_type");
        int localisationId = cmd->getIntParameter("id_localisation");
        std::string releaseCplRefPath = cmd->getStringParameter("cplRefPath");
        
        try {
            TransitionResponse transitionResponse = _boundaryManager.CreateRelease(contentId, typeId, localisationId, releaseCplRefPath);
            response->setDatas(transitionResponse.cmdDatasXML);
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Release created successfully and state machine started");
            
            Poco::Logger::get("ContextCentralThread").information(
                "Release created for content " + std::to_string(contentId) + 
                ", type " + std::to_string(typeId) + 
                ", localisation " + std::to_string(localisationId), __FILE__, __LINE__);
        }
        catch (const std::exception& e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Release creation failed: " + std::string(e.what()));
            response->setDatas("<e><message>" + std::string(e.what()) + "</message></e>");
            
            Poco::Logger::get("ContextCentralThread").error(
                "Error creating release: " + std::string(e.what()), __FILE__, __LINE__);
        }
    }
    else if (cmd->getType() == CommandCentral::CIS_CREATED) {
        int contentId = cmd->getIntParameter("id_content");
        int typeId = cmd->getIntParameter("id_type");
        int localisationId = cmd->getIntParameter("id_localisation");
        std::string releaseCisPath = cmd->getStringParameter("release_cis_path");
        
        try {
            _boundaryManager.ProcessUploadCIS(contentId, typeId, localisationId, releaseCisPath);
            
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("CIS created successfully");
            response->setDatas("<cis path=\"" + releaseCisPath + "\">CIS created</cis>");
            
            Poco::Logger::get("ContextCentralThread").information(
                "CIS created for release " + std::to_string(contentId) + 
                "_" + std::to_string(typeId) + "_" + std::to_string(localisationId) + 
                " with path: " + releaseCisPath, __FILE__, __LINE__);
        }
        catch (const std::exception& e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("CIS creation failed: " + std::string(e.what()));
            response->setDatas("<e><message>" + std::string(e.what()) + "</message></e>");
            
            Poco::Logger::get("ContextCentralThread").error(
                "Error creating CIS: " + std::string(e.what()), __FILE__, __LINE__);
        }
    }
    else if (cmd->getType() == CommandCentral::CREATE_SYNCLOOP) {
        int contentId = cmd->getIntParameter("id_content");
        int typeId = cmd->getIntParameter("id_type");
        int localisationId = cmd->getIntParameter("id_localisation");
        std::string syncLoopPath = cmd->getStringParameter("syncloop_path");

        try {
            _boundaryManager.ProcessUploadSync(contentId, typeId, localisationId, 0, syncLoopPath);
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("SyncLoop uploaded successfully");
            response->setDatas("<syncLoop path=\"" + syncLoopPath + "\">SyncLoop uploaded</syncLoop>");
            Poco::Logger::get("ContextCentralThread").information(
                "SyncLoop uploaded for release " + std::to_string(contentId) +
                "_" + std::to_string(typeId) + "_" + std::to_string(localisationId) +
                " with path: " + syncLoopPath, __FILE__, __LINE__);
        } catch (const std::exception& e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("SyncLoop upload failed: " + std::string(e.what()));
            response->setDatas("<e><message>" + std::string(e.what()) + "</message></e>");
            Poco::Logger::get("ContextCentralThread").error(
                "Error uploading SyncLoop: " + std::string(e.what()), __FILE__, __LINE__);
        }
    }
    else if (cmd->getType() == CommandCentral::CREATE_CPL) {
        int contentId = cmd->getIntParameter("id_content");
        int typeId = cmd->getIntParameter("id_type");
        int localisationId = cmd->getIntParameter("id_localisation");
        
        try {
            _boundaryManager.ProcessNewCPL(contentId, typeId, localisationId);
            
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("CPL creation processed successfully");
            response->setDatas("<cpl>CPL creation processed</cpl>");
            
            Poco::Logger::get("ContextCentralThread").information(
                "CPL creation processed for release " + std::to_string(contentId) + 
                "_" + std::to_string(typeId) + "_" + std::to_string(localisationId), __FILE__, __LINE__);
        }
        catch (const std::exception& e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("CPL creation processing failed: " + std::string(e.what()));
            response->setDatas("<e><message>" + std::string(e.what()) + "</message></e>");
            
            Poco::Logger::get("ContextCentralThread").error(
                "Error processing CPL creation: " + std::string(e.what()), __FILE__, __LINE__);
        }
    }
    else if (cmd->getType() == CommandCentral::CPL_CREATED) {
        int contentId = cmd->getIntParameter("id_content");
        int typeId = cmd->getIntParameter("id_type");
        int localisationId = cmd->getIntParameter("id_localisation");
        int servPairConfigId = cmd->getIntParameter("id_serv_pair_config");
        std::string cplUuid = cmd->getStringParameter("cpl_uuid");
        std::string cplName = cmd->getStringParameter("cpl_name");
        std::string cplPath = cmd->getStringParameter("cpl_path");
        
        try {
            COB_Cpl newCpl(-1, cplName, cplUuid, "", -1, "", cplPath, servPairConfigId, contentId, typeId, localisationId);
            _boundaryManager.GetConfigurator()->GetCplRepo()->Create(&newCpl);
            
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("CPL created successfully");
            response->setDatas(static_cast<std::string>(newCpl));
            
            Poco::Logger::get("ContextCentralThread").information(
                "CPL created for release " + std::to_string(contentId) + 
                "_" + std::to_string(typeId) + "_" + std::to_string(localisationId) + 
                " with UUID: " + cplUuid, __FILE__, __LINE__);
        }
        catch (const std::exception& e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("CPL creation failed: " + std::string(e.what()));
            response->setDatas("<e><message>" + std::string(e.what()) + "</message></e>");
            
            Poco::Logger::get("ContextCentralThread").error(
                "Error creating CPL: " + std::string(e.what()), __FILE__, __LINE__);
        }
    }
    
    /* else if (cmd->getType() == CommandCentral::GET_RELEASE_STATE) {
        int contentId = cmd->getIntParameter("id_content");
        int typeId = cmd->getIntParameter("id_type");
        int localisationId = cmd->getIntParameter("id_localisation");
        
        try {
                         std::string state = _boundaryManager.GetReleaseState(contentId, typeId, localisationId);
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("Release state retrieved successfully");
            response->setDatas("<release_state>" + state + "</release_state>");
        }
        catch (const std::exception& e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("Failed to get release state");
            response->setDatas("<e><message>" + std::string(e.what()) + "</message></e>");
        }
    } */
    else if (cmd->getType() == CommandCentral::GET_RELEASE_CPLS) {
        int contentId = cmd->getIntParameter("id_content");
        int typeId = cmd->getIntParameter("id_type");
        int localisationId = cmd->getIntParameter("id_localisation");
        try {
            // TODO: Implémenter via BoundaryStateManager si nécessaire
            std::string datas = ""; // Placeholder - à implémenter selon le nouveau système
            response->setStatus(CommandCentralResponse::OK);
            response->setComments("CPLs retrieved successfully");
            response->setDatas(datas);
        } catch (const std::exception& e) {
            response->setStatus(CommandCentralResponse::KO);
            response->setComments("CPLs get failed");
            response->setDatas(std::string("<e><message>") + e.what() + "</message></e>");
        }
    }else {
        response->setStatus(CommandCentralResponse::KO);
        response->setComments("Command not found - " + std::to_string(cmd->getType()));
        response->setDatas("<e><message>Command not found</message></e>");
    }
    context->getCommandHandler()->addResponse(response);
}       