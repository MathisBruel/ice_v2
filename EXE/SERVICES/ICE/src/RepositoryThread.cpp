#include "RepositoryThread.h"

RepositoryThread::RepositoryThread()
{
    stop = false;
    thread = nullptr;
}
    
RepositoryThread::~RepositoryThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}

void RepositoryThread::startThread()
{
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    // -- check all is initialized before launch
    LocalRepository* repoLocal = context->getRepoLocal();
    if (repoLocal == nullptr) {
        Poco::Logger::get("RepositoryThread").error("LocalRepository is nullptr !", __FILE__, __LINE__);
        return;
    }

    // -- start thread
    thread = new Poco::Thread("Repository");
    thread->setPriority(Poco::Thread::Priority::PRIO_NORMAL);
    thread->start(*this);
}

void RepositoryThread::stopThread()
{
    stop = true;
    if (thread != nullptr && !thread->tryJoin(2000)) {
        thread->sleep(5000);
        delete thread;
    }
}

void RepositoryThread::run() {

    // -- work every 20 minutes
    int waitTime = 1200000;
    Poco::Stopwatch watch;
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    while (!stop) {
	    
        watch.reset();
        watch.start();

        // -- get context datas
        LocalRepository* repoLocal = context->getRepoLocal();
        CentralRepository* repoCentral = context->getRepoCentral();

        // -- update all scripts information
        while (!context->tryLockLocalRepoMutex()) {usleep(20);}
        repoLocal->updateScriptsInformations();
        context->unlockLocalRepoMutex();

        // -- check if connector working or try reinit 
        if (repoCentral->open()) {
            // -- update list of script available on central repository
            while (!context->tryLockCentralRepoMutex()) {usleep(20);}
            bool availableStatus = repoCentral->checkAvailableScripts();
            context->unlockCentralRepoMutex();

            if (!availableStatus) {
                Poco::Logger::get("RepositoryThread").error("Error when updating list of available scripts", __FILE__, __LINE__);
            }
            else {

                // -- get list of script to have
                while (!context->tryLockCentralRepoMutex()) {usleep(20);}
                std::vector<std::shared_ptr<ScriptInfoFile>> scriptToDownload(repoCentral->getScriptsDownload());
                context->unlockCentralRepoMutex();

                while (!context->tryLockLocalRepoMutex()) {usleep(20);}
                std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>> localScripts(repoLocal->getPathToScripts());
                context->unlockLocalRepoMutex();

                // -- first check script to download
                for (int i = 0; i < scriptToDownload.size(); i++) {

                    std::shared_ptr<ScriptInfoFile> download = scriptToDownload.at(i);

                    // -- trie to found if already existing
                    std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>>::iterator itLocal = localScripts.find(download->getNameScript());

                    // -- not found : create directory if not existing
                    // -- and file INFOS and queue download of files
                    if (itLocal == localScripts.end()) {

                        Poco::Logger::get("RepositoryThread").information("Script do not exist locally : " + download->getNameScript(), __FILE__, __LINE__);

                        // -- define path of directory script
                        Poco::File pathScript(repoLocal->getBasePath() + download->getNameScript());

                        // -- create because not existing
                        if (!pathScript.exists()) {
                            pathScript.createDirectories();
                        }

                        download->save(repoLocal->getBasePath() + download->getNameScript() + "/INFOS");
                        while (!context->tryLockLocalRepoMutex()) {usleep(20);}
                        repoLocal->updateScriptsInformations();
                        context->unlockLocalRepoMutex();

                        // -- add downloads to queue
                        Poco::Logger::get("RepositoryThread").information("Queue download " + download->getSha1Cis() + ".cis", __FILE__, __LINE__);
                        repoCentral->addDownload(download->getFtpPath() + "/" + download->getNameScript() + "/" + download->getSha1Cis() + ".cis", repoLocal->getBasePath() + download->getNameScript() + "/" + download->getSha1Cis() + ".cis");
                        Poco::Logger::get("RepositoryThread").information("Queue download " + download->getSha1Cis() + ".lvi", __FILE__, __LINE__);
                        repoCentral->addDownload(download->getFtpPath() + "/" + download->getNameScript() + "/" + download->getSha1Cis() + ".lvi", repoLocal->getBasePath() + download->getNameScript() + "/" + download->getSha1Cis() + ".lvi");
                        for (int j = 0; j < download->getNbSync(); j++) {
                            Poco::Logger::get("RepositoryThread").information("Queue download " + download->getSyncName(j) + ".sync", __FILE__, __LINE__);
                            repoCentral->addDownload(download->getFtpPath() + "/" + download->getNameScript() + "/" + download->getSyncName(j) + ".sync", repoLocal->getBasePath() + download->getNameScript() + "/" + download->getSyncName(j) + ".sync");
                        }
                    }

                    // -- found : check what file are missing and queue download of files
                    else {
                        Poco::Logger::get("RepositoryThread").information("Script exist locally : " + download->getNameScript(), __FILE__, __LINE__);

                        bool changes = itLocal->second->infos.changeDatas(*download.get());
                        // -- changes detected
                        if (changes) {
                            itLocal->second->infos.save(repoLocal->getBasePath() + download->getNameScript() + "/INFOS");
                            while (!context->tryLockLocalRepoMutex()) {usleep(20);}
                            repoLocal->updateScriptsInformations();
                            context->unlockLocalRepoMutex();
                        }

                        std::shared_ptr<LocalRepository::ScriptsInformations> scriptInfos = itLocal->second;

                        if (!scriptInfos->cisAvailable) {
                            Poco::Logger::get("RepositoryThread").information("Missing : Queue download " + scriptInfos->infos.getSha1Cis() + ".cis", __FILE__, __LINE__);
                            repoCentral->addDownload(scriptInfos->infos.getFtpPath() + "/" + scriptInfos->infos.getNameScript() + "/" + scriptInfos->infos.getSha1Cis() + ".cis", repoLocal->getBasePath() + scriptInfos->infos.getNameScript() + "/" + scriptInfos->infos.getSha1Cis() + ".cis");
                        }
                        if (!scriptInfos->lviAvailable) {
                            Poco::Logger::get("RepositoryThread").information("Missing : Queue download " + scriptInfos->infos.getSha1Cis() + ".lvi", __FILE__, __LINE__);
                            repoCentral->addDownload(scriptInfos->infos.getFtpPath() + "/" + scriptInfos->infos.getNameScript() + "/" + scriptInfos->infos.getSha1Cis() + ".lvi", repoLocal->getBasePath() + scriptInfos->infos.getNameScript() + "/" + scriptInfos->infos.getSha1Cis() + ".lvi");
                        }
                        for (int j = 0; j < scriptInfos->syncPresent.size(); j++) {
                            if (!scriptInfos->syncPresent.at(j)) {
                                Poco::Logger::get("RepositoryThread").information("Missing : Queue download " + scriptInfos->infos.getSyncName(j) + ".sync", __FILE__, __LINE__);
                                repoCentral->addDownload(scriptInfos->infos.getFtpPath() + "/" + scriptInfos->infos.getNameScript() + "/" + scriptInfos->infos.getSyncName(j) + ".sync", repoLocal->getBasePath() + scriptInfos->infos.getNameScript() + "/" + scriptInfos->infos.getSyncName(j) + ".sync");
                            }
                        }
                    }
                }
            
                // -- now check for script to delete (not anymore present on central repo and not to be kept)
                std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>>::iterator itLocal;
                for (itLocal = localScripts.begin(); itLocal != localScripts.end(); itLocal++) {

                    bool found = false;
                    for (int i = 0; i < scriptToDownload.size(); i++) {
                        if (scriptToDownload.at(i)->getNameScript() == itLocal->first) {
                            found = true;
                            break;
                        }
                    }

                    if (!found && !itLocal->second->infos.isToKeep()) {
                        Poco::Logger::get("RepositoryThread").warning("Script do not exist anymore : delete " + itLocal->first, __FILE__, __LINE__);
                        Poco::File dirToDelete(repoLocal->getBasePath() + itLocal->second->infos.getNameScript());
                        dirToDelete.remove(true);
                        while (!context->tryLockLocalRepoMutex()) {usleep(20);}
                        repoLocal->updateScriptsInformations();
                        context->unlockLocalRepoMutex();
                    }
                }
            }
            repoCentral->close();
        }
        else {
            Poco::Logger::get("RepositoryThread").error("Repo central could not be opened !", __FILE__, __LINE__);
        }

        // -- wait for next send request
        long sleepTime = waitTime*1000-watch.elapsed();
        if (sleepTime > 0) {
            usleep(sleepTime);
            
        } else {
            Poco::Logger::get("RepositoryThread").warning("Thread exceeding time of " + std::to_string(-sleepTime) + "µs", __FILE__, __LINE__);
        }
    }
}