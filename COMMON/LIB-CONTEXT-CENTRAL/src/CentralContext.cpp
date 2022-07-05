#include "CentralContext.h"

CentralContext* CentralContext::context = 0;

CentralContext* CentralContext::getCurrentContext()
{
    if (context == nullptr) {
        context = new CentralContext();
    }
    return context;
}

CentralContext::CentralContext()
{
    database = nullptr;
}
CentralContext::~CentralContext()
{
    if (database != nullptr) {
        delete database;
    }
}

bool CentralContext::initDatabase()
{
    // -- init database
    database = new DatabaseConnector();

    if (!database->isSessionActive()) {
        Poco::Logger::get("CentralContext").error("Connection to database mySQL in error", __FILE__, __LINE__);
        return false;
    }

    return true;
}

bool CentralContext::startHttpCmdHandler()
{
    try {
        HttpCentralHandlerFactory* commandHandlerFactory = new HttpCentralHandlerFactory();
        Poco::Net::HTTPServer* server = new Poco::Net::HTTPServer(commandHandlerFactory, 8500, new Poco::Net::HTTPServerParams);
        server->start();

    } catch (std::exception &e) {
        Poco::Logger::get("CentralContext").error("Error at initialization of http command receiver !", __FILE__, __LINE__);
        return false;
    }

    return true;
}

ResultQuery* CentralContext::executeQuery(Query* query)
{
    while (!tryLockDatabaseMutex()) {Timer::crossUsleep(15);}
    ResultQuery *result = database->executeQuery(query);
    unlockDatabaseMutex();
    return result;
}

void CentralContext::addStateAuditorium(std::shared_ptr<StateServer> state)
{
    while (!mutexState.tryLock()) {Timer::crossUsleep(15);}
    stateAuditoriums.insert_or_assign(state->getId(), state);
    mutexState.unlock();
}
void CentralContext::addStateServer(std::shared_ptr<StateServer> state)
{
    while (!mutexState.tryLock()) {Timer::crossUsleep(15);}
    stateServers.insert_or_assign(state->getId(), state);
    mutexState.unlock();
}
void CentralContext::removeStateAuditorium(int id_auditorium)
{
    while (!mutexState.tryLock()) {Timer::crossUsleep(15);}
    stateAuditoriums.erase(id_auditorium);
    mutexState.unlock();
}
void CentralContext::removeStateServer(int id_server)
{
    while (!mutexState.tryLock()) {Timer::crossUsleep(15);}
    stateServers.erase(id_server);
    mutexState.unlock();
}

void CentralContext::addDownloadedScripts(std::shared_ptr<DownloadedScripts> downloaded)
{
    while (!mutexServerData.tryLock()) {Timer::crossUsleep(15);}
    scripts.insert_or_assign(downloaded->getIdAuditorium(), downloaded);
    mutexServerData.unlock();
}
void CentralContext::addPresentCpls(std::shared_ptr<PresentCpl> presents)
{
    while (!mutexServerData.tryLock()) {Timer::crossUsleep(15);}
    cpls.insert_or_assign(presents->getIdAuditorium(), presents);
    mutexServerData.unlock();
}
void CentralContext::removeDownloadedScripts(int id_server)
{
    while (!mutexServerData.tryLock()) {Timer::crossUsleep(15);}
    scripts.erase(id_server);
    mutexServerData.unlock();
}
void CentralContext::removePresentCpls(int id_server)
{
    while (!mutexServerData.tryLock()) {Timer::crossUsleep(15);}
    cpls.erase(id_server);
    mutexServerData.unlock();
}

void CentralContext::addScriptsInfos(int id_release, std::shared_ptr<ScriptState> info)
{
    while (!mutexReleaseData.tryLock()) {Timer::crossUsleep(15);}
    scriptServerForRelease.insert_or_assign(id_release, info);
    mutexReleaseData.unlock();
}
void CentralContext::removeScriptsInfos(int id_release)
{
    while (!mutexReleaseData.tryLock()) {Timer::crossUsleep(15);}
    scriptServerForRelease.erase(id_release);
    mutexReleaseData.unlock();
}
void CentralContext::addOutputSynchro(std::string uuidCpl, std::string output)
{
    while (!mutexReleaseData.tryLock()) {Timer::crossUsleep(15);}
    outputsSynchro.insert_or_assign(uuidCpl, output);
    mutexReleaseData.unlock();
}
void CentralContext::removeOutputSynchro(std::string uuidCpl)
{
    while (!mutexReleaseData.tryLock()) {Timer::crossUsleep(15);}
    outputsSynchro.erase(uuidCpl);
    mutexReleaseData.unlock();
}

std::map<int, std::shared_ptr<Auditorium>> CentralContext::prepareAuditoriums()
{
    Query* queryAudis = Auditorium::getQuery();
    ResultQuery *resultAudis = executeQuery(queryAudis);
    Query* queryServers = Server::getQuery();
    ResultQuery *resultServers = executeQuery(queryServers);

    if (!resultAudis->isValid() || !resultServers->isValid()) {

        delete queryAudis;
        delete queryServers;
        delete resultAudis;
        delete resultServers;
        return std::map<int, std::shared_ptr<Auditorium>>();
    }
    else {
        std::map<int, std::shared_ptr<Auditorium>> audis = Auditorium::loadListFromResult(resultAudis);
        std::map<int, std::shared_ptr<Auditorium>>::iterator itAudi;
        std::map<int, std::shared_ptr<Server>> servers = Server::loadListFromResult(resultServers);
        std::map<int, std::shared_ptr<Server>>::iterator itServer;

        for (itServer = servers.begin(); itServer != servers.end(); itServer++) {

            itAudi = audis.find(itServer->second->getIdAuditorium());
            if (itAudi != audis.end()) {
                if (itServer->second->getTypeServer() == Server::FEATURE) {
                    itAudi->second->setFeatureServer(itServer->second);
                }
                else if (itServer->second->getTypeServer() == Server::SAS) {
                    itAudi->second->setSasServer(itServer->second);
                }
            }
        }

        delete queryAudis;
        delete queryServers;
        delete resultAudis;
        delete resultServers;
        return audis;
    } 
}
std::map<int, std::shared_ptr<Server>> CentralContext::prepareServers()
{
    Query* queryServers = Server::getQuery();
    ResultQuery *resultServers = executeQuery(queryServers);

    if (!resultServers->isValid()) {

        delete queryServers;
        delete resultServers;
        return std::map<int, std::shared_ptr<Server>>();
    }
    else {
        std::map<int, std::shared_ptr<Server>> servers = Server::loadListFromResult(resultServers);
        delete queryServers;
        delete resultServers;
        return servers;
    } 
}

std::map<int, std::shared_ptr<Release>> CentralContext::prepareRelease()
{
    Query* queryRelease = Release::getQuery();
    ResultQuery *resultRelease = executeQuery(queryRelease);
    Query* queryCuts = ReleaseCut::getQuery();
    ResultQuery *resultCuts = executeQuery(queryCuts);
    Query* queryScript = Script::getQuery();
    ResultQuery *resultScript = executeQuery(queryScript);
    Query* queryCpls = Cpl::getQuery();
    ResultQuery *resultCpls = executeQuery(queryCpls);
    Query* queryLinkCplRelease = LinkParam::getQuery(LinkParam::CPL_RELEASE);
    ResultQuery *resultLinkCplRelease = executeQuery(queryLinkCplRelease);
    Query* queryLinkScriptRelease = LinkParam::getQuery(LinkParam::SCRIPT_RELEASE);
    ResultQuery *resultLinkScriptRelease = executeQuery(queryLinkScriptRelease);

    if (!resultRelease->isValid() || !resultCuts->isValid() ||
        !resultScript->isValid() || !resultCpls->isValid() || 
        !resultLinkCplRelease->isValid() || !resultLinkScriptRelease->isValid()) {

        delete queryRelease;
        delete queryCuts;
        delete queryScript;
        delete queryCpls;
        delete queryLinkCplRelease;
        delete queryLinkScriptRelease;
        delete resultRelease;
        delete resultCuts;
        delete resultScript;
        delete resultCpls;
        delete resultLinkCplRelease;
        delete resultLinkScriptRelease;
        return std::map<int, std::shared_ptr<Release>>();
    }
    else {
        static std::map<int, std::shared_ptr<Release>> releases = Release::loadListFromResult(resultRelease);
        std::map<int, std::shared_ptr<Release>>::iterator itRelease;
        std::map<int, std::shared_ptr<ReleaseCut>> cuts = ReleaseCut::loadListFromResult(resultCuts);
        std::map<int, std::shared_ptr<ReleaseCut>>::iterator itCut;
        std::map<int, std::shared_ptr<Script>> scripts = Script::loadListFromResult(resultScript);
        std::map<int, std::shared_ptr<Script>>::iterator itScript;
        std::map<int, std::shared_ptr<Cpl>> cpls = Cpl::loadListFromResult(resultCpls);
        std::vector<std::shared_ptr<LinkParam>> linksCpl = LinkParam::loadListFromResult(resultLinkCplRelease, LinkParam::CPL_RELEASE);
        std::vector<std::shared_ptr<LinkParam>> linksScript = LinkParam::loadListFromResult(resultLinkScriptRelease, LinkParam::SCRIPT_RELEASE);

        for (itRelease = releases.begin(); itRelease != releases.end(); itRelease++) {
            
            // -- add cuts
            for (itCut = cuts.begin(); itCut != cuts.end(); itCut++) {
                if (itCut->second->getIdRelease() == itRelease->first) itRelease->second->addCut(itCut->second);
            }

            // -- add scripts
            for (std::shared_ptr<LinkParam> link : linksScript) {

                if (link->getDstId() == itRelease->first) {
                    itScript = scripts.find(link->getSrcId()); 
                    if (itScript != scripts.end()) {
                        if (itScript->second->getType() == Script::FEATURE) {
                            itRelease->second->setFeatureScript(itScript->second);
                        }
                        else if (itScript->second->getType() == Script::LOOP) {
                            itRelease->second->setLoopScript(itScript->second);
                        }
                        else if (itScript->second->getType() == Script::SAS) {
                            itRelease->second->setSasScript(itScript->second);
                        }
                    }
                }
            }

            // -- set parent
            if (itRelease->second->getIdParent() != -1) {
                std::map<int, std::shared_ptr<Release>>::iterator itParent = releases.find(itRelease->second->getIdParent());
                if (itParent != releases.end()) {
                    itRelease->second->setParentRelease(itParent->second);
                }
                else {
                    Poco::Logger::get("CentralContext").error("Parent release not found with id : " + std::to_string(itRelease->second->getIdParent()), __FILE__, __LINE__);
                }
            }
        }

        for (std::shared_ptr<LinkParam> link : linksCpl) {

            if (link->getDstId() == itRelease->first) {
                std::map<int, std::shared_ptr<Cpl>>::iterator itCpl = cpls.find(link->getSrcId()); 
                if (itCpl != cpls.end()) {
                    itRelease->second->addFeatureCpl(itCpl->second);
                }
            }
        }


        delete queryRelease;
        delete queryCuts;
        delete queryScript;
        delete queryCpls;
        delete queryLinkCplRelease;
        delete resultRelease;
        delete resultCuts;
        delete resultScript;
        delete resultCpls;
        delete resultLinkCplRelease;
        return releases;
    }
}
std::map<int, std::shared_ptr<Cinema>> CentralContext::prepareCinemas() 
{
    Query* queryCinema = Cinema::getQuery();
    ResultQuery *resultCinema = executeQuery(queryCinema);
    Query* queryAudis = Auditorium::getQuery();
    ResultQuery *resultAudis = executeQuery(queryAudis);
    Query* queryServers = Server::getQuery();
    ResultQuery *resultServers = executeQuery(queryServers);

    if (!resultCinema->isValid() || !resultAudis->isValid() ||
        !resultServers->isValid()) {

        delete queryCinema;
        delete queryAudis;
        delete queryServers;
        delete resultCinema;
        delete resultAudis;
        delete resultServers;
        return std::map<int, std::shared_ptr<Cinema>>();
    }
    else {
        std::map<int, std::shared_ptr<Cinema>> cinemas = Cinema::loadListFromResult(resultCinema);
        std::map<int, std::shared_ptr<Cinema>>::iterator itCinema;
        std::map<int, std::shared_ptr<Auditorium>> auditoriums = Auditorium::loadListFromResult(resultAudis);
        std::map<int, std::shared_ptr<Auditorium>>::iterator itAudi;
        std::map<int, std::shared_ptr<Server>> servers = Server::loadListFromResult(resultCinema);
        std::map<int, std::shared_ptr<Server>>::iterator itServer;

        for (itCinema = cinemas.begin(); itCinema != cinemas.end(); itCinema++) {

            for (itAudi = auditoriums.begin(); itAudi != auditoriums.end(); itAudi++) {
                if (itAudi->second->getIdCinema() == itCinema->first) {
                    itCinema->second->addAuditorium(itAudi->second);
                }

                for (itServer = servers.begin(); itServer != servers.end(); itServer++) {
                    if (itServer->second->getIdAuditorium() == itAudi->first) {
                        if (itServer->second->getTypeServer() == Server::FEATURE) {
                            itAudi->second->setFeatureServer(itServer->second);
                        }
                        else if (itServer->second->getTypeServer() == Server::SAS) {
                            itAudi->second->setSasServer(itServer->second);
                        }
                    }
                }
            }
        }

        delete queryCinema;
        delete queryAudis;
        delete queryServers;
        delete resultCinema;
        delete resultAudis;
        delete resultServers;
        return cinemas;
    }
}

std::vector<int> CentralContext::associateRelatedCinema(std::shared_ptr<Release> release)
{
    int id_release = release->getId();
    Query* queryLinkGroupRelease = LinkParam::getQueryDst(LinkParam::GROUP_RELEASE, &id_release);
    ResultQuery *resultLinkGroupRelease = executeQuery(queryLinkGroupRelease);
    Query* queryLinkCinemaRelease = LinkParam::getQueryDst(LinkParam::CINEMA_RELEASE, &id_release);
    ResultQuery *resultLinkCinemaRelease = executeQuery(queryLinkCinemaRelease);
    Query* queryLinkCinemaGroup = LinkParam::getQuery(LinkParam::CINEMA_GROUP);
    ResultQuery *resultLinkCinemaGroup = executeQuery(queryLinkCinemaGroup);

    if (!resultLinkGroupRelease->isValid() ||
        !resultLinkGroupRelease->isValid() ||
        !resultLinkGroupRelease->isValid()) {

        delete queryLinkGroupRelease;
        delete queryLinkCinemaRelease;
        delete queryLinkCinemaGroup;
        delete resultLinkGroupRelease;
        delete resultLinkCinemaRelease;
        delete resultLinkCinemaGroup;
        return std::vector<int>();
    }
    else {
        
        std::vector<std::shared_ptr<LinkParam>> linkCinemaGroups = LinkParam::loadListFromResult(resultLinkCinemaGroup, LinkParam::CINEMA_GROUP);
        std::vector<std::shared_ptr<LinkParam>> linkGroupReleases = LinkParam::loadListFromResult(resultLinkGroupRelease, LinkParam::GROUP_RELEASE);
        std::vector<std::shared_ptr<LinkParam>> linkCinemaReleases = LinkParam::loadListFromResult(resultLinkCinemaGroup, LinkParam::CINEMA_RELEASE);
        std::vector<int> toReturn;

        for (std::shared_ptr<LinkParam> linkGroupRelease : linkGroupReleases) {
            for (std::shared_ptr<LinkParam> linkCinemaGroup : linkCinemaGroups) {
                if (linkCinemaGroup->getDstId() == linkGroupRelease->getSrcId()) {

                    if (std::find(toReturn.begin(), toReturn.end(), linkCinemaGroup->getSrcId()) == toReturn.end()) {
                        toReturn.push_back(linkCinemaGroup->getSrcId());
                    }
                }
            }
        }

        for (std::shared_ptr<LinkParam> linkCinemaRelease : linkCinemaReleases) {
            if (std::find(toReturn.begin(), toReturn.end(), linkCinemaRelease->getSrcId()) == toReturn.end()) {
                toReturn.push_back(linkCinemaRelease->getSrcId());
            }
        }

        delete queryLinkGroupRelease;
        delete queryLinkCinemaRelease;
        delete queryLinkCinemaGroup;
        delete resultLinkGroupRelease;
        delete resultLinkCinemaRelease;
        delete resultLinkCinemaGroup;
        return toReturn;
    }
}