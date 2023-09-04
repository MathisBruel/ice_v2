#include "RepositoryCommandThread.h"

RepositoryCommandThread::RepositoryCommandThread()
{
    stop = false;
    thread = nullptr;
}
    
RepositoryCommandThread::~RepositoryCommandThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}

void RepositoryCommandThread::startThread()
{
    // -- check all is initialized before launch
    LocalRepository* repoLocal = ApplicationContext::getCurrentContext()->getRepoLocal();
    if (repoLocal == nullptr) {
        Poco::Logger::get("RepositoryCommandThread").error("LocalRepository is nullptr !", __FILE__, __LINE__);
        return;
    }

    // -- check all is initialized before launch
    CentralRepository* repoCentral = ApplicationContext::getCurrentContext()->getRepoCentral();
    if (repoCentral == nullptr) {
        Poco::Logger::get("RepositoryCommandThread").error("LocalRepository is nullptr !", __FILE__, __LINE__);
        return;
    }

    // -- start thread
    thread = new Poco::Thread("RepositoryCommand");
    thread->setPriority(Poco::Thread::Priority::PRIO_NORMAL);
    thread->start(*this);
}

void RepositoryCommandThread::addCommand(std::shared_ptr<Command> cmd)
{
    while (!commandsMutex.tryLock()) {usleep(20);}
    commands.insert_or_assign(cmd->getUuid(), cmd);
    commandsMutex.unlock();
}

void RepositoryCommandThread::run() {

    // -- work every 500 ms
    int waitTime = 500;
    Poco::Stopwatch watch;

    while (!stop) {
	    
        watch.reset();
        watch.start();

        // -- execute command in time
        long sleepTime = waitTime*1000-watch.elapsed();

        while (!commandsMutex.tryLock()) {usleep(20);}
        int sizeCommands = commands.size();
        commandsMutex.unlock();

        // -- treat nesting cmd if time
        while (sleepTime >= 1000 && sizeCommands > 0) {

            while (!commandsMutex.tryLock()) {usleep(20);}
            std::map<std::string, std::shared_ptr<Command>>::iterator it = commands.begin();
            commandsMutex.unlock();

            std::shared_ptr<Command> cmd = it->second;
            executeCommand(cmd);

            while (!commandsMutex.tryLock()) {usleep(20);}
            commands.erase(it);
            sizeCommands = commands.size();
            commandsMutex.unlock();

            sleepTime = waitTime*1000-watch.elapsed();
        }

        // -- wait for next send request
        if (sleepTime > 0) {
            usleep(sleepTime);
            
        } else {
            Poco::Logger::get("RepositoryCommandThread").warning("Thread exceeding time of " + std::to_string(-sleepTime) + "µs", __FILE__, __LINE__);
        }
    }
}

void RepositoryCommandThread::executeCommand(std::shared_ptr<Command> cmd)
{
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    // -- create response to CMD
    std::shared_ptr<RepositoryResponse> response = std::make_shared<RepositoryResponse>(RepositoryResponse(cmd->getUuid(), cmd->getType()));
    LocalRepository* repoLocal = context->getRepoLocal();

    std::string scriptName = cmd->getStringParameter("scriptName");

    if (cmd->getType() == Command::GET_SYNC_CONTENT) {

        std::string idSync = cmd->getStringParameter("syncId");

        while (!context->tryLockLocalRepoMutex()) {usleep(20);}
        std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>> mapScripts(repoLocal->getCplIdToscripts());
        context->unlockLocalRepoMutex();

        std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>>::iterator it = mapScripts.find(idSync);

        if (it == mapScripts.end()) {
            response->setStatus(CommandResponse::KO);
            response->setComments("Sync file " + idSync + " not found !");
            return;
        }
        
        std::string filePath = repoLocal->getBasePath() + it->second->infos.getNameScript() + "/" + idSync + ".sync";
        Poco::File pathFile(filePath);

        if (!pathFile.exists()) {
            response->setStatus(CommandResponse::KO);
            response->setComments("Sync file " + idSync + " do not exist !");
            return;
        }

        // -- read whole file
        std::ifstream ifs(filePath);
        std::string content;
        if(ifs) {
            std::ostringstream ss;
            ss << ifs.rdbuf(); // reading data
            content = ss.str();
        }
        ifs.close();

        response->setStatus(CommandResponse::OK);
        response->setComments("Sync " + filePath + " getted !");
        response->setContentSync(content);
    }
    else if (cmd->getType() == Command::CHANGE_PRIORITY_SCRIPT) {

        while (!context->tryLockLocalRepoMutex()) {usleep(20);}
        bool priority = cmd->getBoolParameter("priority");
        if (repoLocal->setPriorityScript(priority, scriptName)) {
            response->setStatus(CommandResponse::OK);
            response->setComments("Script " + scriptName + " set to priority download !");
        }
        else {
            response->setStatus(CommandResponse::KO);
            response->setComments("Script " + scriptName + " not found or could not create KEEP file !");
        }
        context->unlockLocalRepoMutex();
    }
    else if (cmd->getType() == Command::SYNCHRONIZE_SCRIPT) {
        std::string idSync = cmd->getStringParameter("sync");
        std::string idCpl = cmd->getStringParameter("cpl");
        std::string method = cmd->getStringParameter("method", "byId");

        bool syncFileOk = false;

        if (idSync == "" && scriptName == "") {
            response->setStatus(CommandResponse::KO);
            response->setComments("Script and sync not found !");
        }
        // -- search sync reference
        else if (idSync == "") {
            while (!context->tryLockLocalRepoMutex()) {usleep(20);}
            std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>> mapScripts = repoLocal->getPathToScripts();
            context->unlockLocalRepoMutex();

            std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>>::iterator it;
            for (it = mapScripts.begin(); it != mapScripts.end(); it++) {
                
                if (it->first.find(scriptName) != std::string::npos) {

                    std::string lviPath = repoLocal->getBasePath() + it->second->infos.getNameScript() + "/" + it->second->infos.getSha1Cis() + ".lvi";
                    LVI lvi(lviPath);
                    if (lvi.load()) {
                        idSync = lvi.getCplId();
                        syncFileOk = true;
                    }
                    break;
                }
            }
        }
        else {
            syncFileOk = true;
        }

        if (syncFileOk) {

            if (method == "byId") {
                synchronizeScriptById(idSync, idCpl, response.get());
            }
            else {
                synchronizeScriptByDuration(idSync, idCpl, response.get());
            }
        }
        else {
            response->setStatus(CommandResponse::KO);
            response->setComments("Id sync is not correct !");
        }
    }

    context->getCommandHandler()->addResponse(response);
}

void RepositoryCommandThread::synchronizeScriptById(std::string idSyncRef, std::string idCplNew, RepositoryResponse* response)
{
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    LocalRepository* repoLocal = context->getRepoLocal();

    // -- create request to IMS to get content of CPL
    std::shared_ptr<Command> cmdGetCplContent = std::make_shared<Command>(Command(Command::GET_CPL_CONTENT));
    cmdGetCplContent->addParameter("cplId", idCplNew);

    context->getCommandHandler()->addCommand(cmdGetCplContent);

    // -- waiting for response (3 retry with 300ms each)
    int retry = 3;
    std::shared_ptr<CommandResponse> responseGetCpl = context->getCommandHandler()->getResponse(cmdGetCplContent->getUuid());
    while (responseGetCpl == nullptr || retry > 0) {
        usleep(300000);
        retry--;
        responseGetCpl = context->getCommandHandler()->getResponse(cmdGetCplContent->getUuid());
    }

    // -- check if get response
    if (responseGetCpl == nullptr) {
        response->setStatus(CommandResponse::KO);
        response->setComments("Could not get a response from IMS");
    }
    else {

        // -- check response status
        if (responseGetCpl->getStatus() == CommandResponse::KO) {
            response->setStatus(CommandResponse::KO);
            response->setComments("Response from IMS is KO");
        }
        // -- check response type
        else if (responseGetCpl->getType() != Command::GET_CPL_CONTENT) {
            response->setStatus(CommandResponse::KO);
            response->setComments("Response from IMS is not the correct type");
        }
        // -- treat response
        else {
            ImsResponse* imsResp = static_cast<ImsResponse*>(responseGetCpl.get());
            std::shared_ptr<CplFile> cplToSync = std::make_shared<CplFile>(CplFile());
            
            // -- load CPL from response to IMS
            if (!cplToSync->loadCplFromContentString(imsResp->getContentFile())) {
                response->setStatus(CommandResponse::KO);
                response->setComments("Could not parse cpl file");
                context->getCommandHandler()->deleteResponse(responseGetCpl->getUuid());
                return;
            }
            else {

                context->getCommandHandler()->deleteResponse(responseGetCpl->getUuid());

                // -- tries to find sync file
                while (!context->tryLockLocalRepoMutex()) {usleep(20);}
                std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>> mapScripts = repoLocal->getCplIdToscripts();
                context->unlockLocalRepoMutex();

                std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>>::iterator it;
                it = mapScripts.find(idSyncRef);
                std::string pathScript = "";
                if (it == mapScripts.end()) {
                    response->setStatus(CommandResponse::KO);
                    response->setComments("Could not find sync file locally to synchronize from!");
                }
                else {
                    pathScript = repoLocal->getBasePath() + it->second->infos.getNameScript() + "/";
                }

                // -- not found
                if (pathScript != "") {
                    // -- load sync file
                    Poco::Path pathSyncFile(pathScript);
                    pathSyncFile.setFileName(idSyncRef + ".sync");
                    std::shared_ptr<SyncFile> syncFile = std::make_shared<SyncFile>(SyncFile(pathSyncFile.toString()));
                    
                    
                    // -- load OK
                    if (syncFile->load()) {

                        // -- tries to synchronize
                        Synchro synchronizer(syncFile.get(), cplToSync.get(), nullptr);
                        
                        synchronizer.tryFindSamePictureId();
                        std::vector<Synchro::Match2> matches = synchronizer.getMatch2();

                        // -- check at least one match exists
                        if (matches.size() <= 0) {
                            response->setStatus(CommandResponse::KO);
                            response->setComments("No match on id of Reel !");
                            return;
                        }
                        
                        synchronizer.checkFilled2();

                        // -- give informations to response
                        response->setReelsSyncRef(synchronizer.getSynchronizable()->getReels());
                        response->setReelsCplNew(synchronizer.getNewCpl()->getReels());
                        response->setMatchesById(synchronizer.getMatch2());
                        response->setInterMatches(synchronizer.getInter());
                        response->setInvalidCuts(synchronizer.getInvalidCuts());
                        response->setById(true);

                        if (!synchronizer.isSynchronized()) {
                            response->setStatus(CommandResponse::KO);
                            response->setComments("Synchronization failed !");
                        }

                        // -- synchronization OK
                        else {
                            Poco::Path pathNewSyncFile(pathScript);
                            pathNewSyncFile.setFileName(idCplNew + ".sync");
                            SyncFile* newSyncFile = synchronizer.generateSyncFile(pathNewSyncFile.toString());

                            while (!context->tryLockLocalRepoMutex()) {usleep(20);}
                            if (newSyncFile == nullptr) {
                                response->setStatus(CommandResponse::KO);
                                response->setComments("Generation of new synced file failed !");
                            }
                            else if (!newSyncFile->save()) {
                                response->setStatus(CommandResponse::KO);
                                response->setComments("Save of new synced file failed !");
                            }
                            else {
                                repoLocal->updateScriptsInformations();
                                response->setStatus(CommandResponse::OK);
                                response->setComments("Generation of synchro achieved !");
                            }
                            context->unlockLocalRepoMutex();

                            delete newSyncFile;
                        }
                    }

                    // -- load KO
                    else {
                        response->setStatus(CommandResponse::KO);
                        response->setComments("Load of sync file failed !");
                    }
                }
            }
        }
    }
}

void RepositoryCommandThread::synchronizeScriptByDuration(std::string idSyncRef, std::string idCplNew, RepositoryResponse* response)
{
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    LocalRepository* repoLocal = context->getRepoLocal();

    // -- create request to IMS to get content of CPL
    std::shared_ptr<Command> cmdGetCplContent = std::make_shared<Command>(Command(Command::GET_CPL_CONTENT));
    cmdGetCplContent->addParameter("cplId", idCplNew);

    context->getCommandHandler()->addCommand(cmdGetCplContent);

    // -- waiting for response (3 retry with 300ms each)
    int retry = 3;
    std::shared_ptr<CommandResponse> responseGetCpl = context->getCommandHandler()->getResponse(cmdGetCplContent->getUuid());
    while (responseGetCpl == nullptr || retry > 0) {
        usleep(300000);
        retry--;
        responseGetCpl = context->getCommandHandler()->getResponse(cmdGetCplContent->getUuid());
    }

    // -- check if get response
    if (responseGetCpl == nullptr) {
        response->setStatus(CommandResponse::KO);
        response->setComments("Could not get a response from IMS");
    }
    else {

        // -- check response status
        if (responseGetCpl->getStatus() == CommandResponse::KO) {
            response->setStatus(CommandResponse::KO);
            response->setComments("Response from IMS is KO");
        }
        // -- check response type
        else if (responseGetCpl->getType() != Command::GET_CPL_CONTENT) {
            response->setStatus(CommandResponse::KO);
            response->setComments("Response from IMS is not the correct type");
        }
        // -- treat response
        else {
            ImsResponse* imsResp = static_cast<ImsResponse*>(responseGetCpl.get());
            std::shared_ptr<CplFile> cplToSync = std::make_shared<CplFile>(CplFile());
            
            // -- load CPL from response to IMS
            if (!cplToSync->loadCplFromContentString(imsResp->getContentFile())) {
                response->setStatus(CommandResponse::KO);
                response->setComments("Could not parse cpl file");
                context->getCommandHandler()->deleteResponse(responseGetCpl->getUuid());
                return;
            }
            else {

                context->getCommandHandler()->deleteResponse(responseGetCpl->getUuid());

                while (!context->tryLockLocalRepoMutex()) {usleep(20);}
                std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>> mapScripts = repoLocal->getCplIdToscripts();
                context->unlockLocalRepoMutex();

                std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>>::iterator it;
                it = mapScripts.find(idSyncRef);
                std::string pathScript = "";
                if (it == mapScripts.end()) {
                    response->setStatus(CommandResponse::KO);
                    response->setComments("Could not find sync file locally to synchronize from!");
                }
                else {
                    pathScript = repoLocal->getBasePath() + it->second->infos.getNameScript() + "/";
                }        


                if (pathScript != "") {
                    // -- load sync file
                    Poco::Path pathSyncFile(pathScript);
                    pathSyncFile.setFileName(idSyncRef + ".sync");
                    std::shared_ptr<SyncFile> syncFile = std::make_shared<SyncFile>(SyncFile(pathSyncFile.toString()));
                    
                    // -- load OK
                    if (syncFile->load()) {

                        // -- tries to synchronize
                        Synchro synchronizer(syncFile.get(), cplToSync.get(), nullptr);
                        
                        synchronizer.tryMatchMulti();
                        std::vector<Synchro::Match> matches = synchronizer.getMatch();

                        // -- check at least one match exists
                        if (matches.size() <= 0) {
                            response->setStatus(CommandResponse::KO);
                            response->setComments("No match on duration of Reel !");
                            return;
                        }
                        
                        synchronizer.fillMatches();
                        synchronizer.checkFilled();

                        // -- give informations to response
                        response->setReelsSyncRef(synchronizer.getSynchronizable()->getReels());
                        response->setReelsCplNew(synchronizer.getNewCpl()->getReels());
                        response->setMatchesByDuration(synchronizer.getMatch());
                        response->setInterMatches(synchronizer.getInter());
                        response->setById(false);

                        if (!synchronizer.isSynchronized()) {
                            response->setStatus(CommandResponse::KO);
                            response->setComments("Synchronization failed !");
                        }

                        // -- synchronization OK
                        else {
                            Poco::Path pathNewSyncFile(pathScript);
                            pathNewSyncFile.setFileName(idCplNew + ".sync");
                            SyncFile* newSyncFile = synchronizer.generateSyncFile(pathNewSyncFile.toString());

                            while (!context->tryLockLocalRepoMutex()) {usleep(20);}
                            if (newSyncFile == nullptr) {
                                response->setStatus(CommandResponse::KO);
                                response->setComments("Generation of new synced file failed !");
                            }
                            else if (!newSyncFile->save()) {
                                response->setStatus(CommandResponse::KO);
                                response->setComments("Save of new synced file failed !");
                            }
                            else {
                                repoLocal->updateScriptsInformations();
                                response->setStatus(CommandResponse::OK);
                                response->setComments("Generation of synchro achieved !");
                            }
                            context->unlockLocalRepoMutex();

                            delete newSyncFile;
                        }
                    }

                    // -- load KO
                    else {
                        response->setStatus(CommandResponse::KO);
                        response->setComments("Load of sync file failed !");
                    }
                }
            }
        }
    }
}