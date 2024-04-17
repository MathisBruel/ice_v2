#include "SynchroThread.h"

SynchroThread::SynchroThread()
{
    stop = false;
    thread = nullptr;
}
SynchroThread::~SynchroThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}
void SynchroThread::startThread()
{
    thread = new Poco::Thread("SynchroThread");
    thread->setPriority(Poco::Thread::Priority::PRIO_HIGH);
    thread->start(*this);
    Poco::Logger::get("SynchroThread").debug("Thread id : " + std::to_string(thread->currentTid()), __FILE__, __LINE__);
}
void SynchroThread::run()
{
    Poco::Stopwatch watch;
    CentralContext* context = CentralContext::getCurrentContext();

    // -- works at 5 min rate
    int waitTime = 300000;
    while (!stop) {
	    
        watch.reset();
        watch.start();

        std::map<int, std::shared_ptr<Release>> releases(context->prepareRelease());

        std::map<int, std::shared_ptr<Release>>::iterator itReleases;
        for (itReleases = releases.begin(); itReleases != releases.end(); itReleases++) {

            std::shared_ptr<Release> release = itReleases->second;
            Poco::DateTime currentDate;
            int64_t timestampCurrent = currentDate.timestamp().epochMicroseconds();
            int64_t inDate = release->getInDate()->timestamp().epochMicroseconds();

            // -- valid if already available or available at least in the next 7 days
            if ((inDate - timestampCurrent) > 604800000) {
                Poco::Logger::get("SynchroThread").information("Release " + release->getName() + " ignored : start in more than 7 days !", __FILE__, __LINE__);
                continue;
            }

            // -- valid 24 hours after end validity
            std::shared_ptr<Poco::DateTime> outDatetime = release->getOutDate();
            if (outDatetime != nullptr) {
                int64_t outDate = outDatetime->timestamp().epochMicroseconds();
                if ((timestampCurrent - outDate) > 86400000) {
                    Poco::Logger::get("SynchroThread").information("Release " + release->getName() + " ignored : ended more than 24 hours !", __FILE__, __LINE__);
                    continue;
                }
            }

            // -- check cpl not already synchronized
            std::vector<std::shared_ptr<Cpl>> featureCpls = release->getFeatureCpls();
            for (std::shared_ptr<Cpl> featureCpl : featureCpls) {

                if (!featureCpl->getSyncPath().empty()) {
                    Poco::Logger::get("SynchroThread").information("Cpl " + featureCpl->getName() + " ignored : sync already done !", __FILE__, __LINE__);
                    continue;
                }

                // -- level 0 synchro
                std::shared_ptr<Script> startScript = release->getFeatureScript();
                if (startScript != nullptr) {

                    Poco::Logger::get("SynchroThread").information("Script present in release !", __FILE__, __LINE__);

                    Poco::File pathScript(startScript->getPath());
                    std::vector<Poco::File> listFiles;
                    pathScript.list(listFiles);
                
                    std::string pathlvi = "";
                    for (int i = 0; i < listFiles.size(); i++) {
                        Poco::Path pathFile(listFiles.at(i).path());
                        if (pathFile.getExtension() == "lvi") {
                            pathlvi = pathFile.toString();
                            break;
                        }
                    }

                    if (pathlvi.empty()) {
                        Poco::Logger::get("SynchroThread").error("Lvi of script not present !", __FILE__, __LINE__);
                        break;
                    }
                    LVI* lviRef = new LVI(pathlvi);
                    if (!lviRef->load()) {
                        Poco::Logger::get("SynchroThread").error("Could not load lvi file !", __FILE__, __LINE__);
                        break;
                    }

                    Poco::File xmlCpl(featureCpl->getCplPath());
                    if (!xmlCpl.exists()) {
                        Poco::Logger::get("SynchroThread").error("Cpl path does not exist !", __FILE__, __LINE__);
                        continue;
                    }

                    CplFile* cplToSync = new CplFile(featureCpl->getCplPath());
                    if (!cplToSync->load()) {
                        Poco::Logger::get("SynchroThread").error("Could not load cpl file !", __FILE__, __LINE__);
                        continue;
                    }

                    // -- prepare cuts
                    std::vector<std::shared_ptr<ReleaseCut>> cuts(release->getCuts());
                    CutScenes* cutsFinal = new CutScenes(cplToSync->getCplId(), cplToSync->getCplTitle(), lviRef->getCplId(), lviRef->getCplTitle());
                    for (std::shared_ptr<ReleaseCut> cut : cuts) {
                        cutsFinal->addCut(cut->getPosition(), cut->getPosition()+cut->getSize()-1, cut->getDescription());
                    }

                    // -- DO the synchro (only by duration : by id is done through manual synchro)
                    std::shared_ptr<Synchro> synchronizer = std::make_shared<Synchro>(Synchro(lviRef, cplToSync, cutsFinal));
                    synchronizer->synchronizeByDuration();

                    // -- save outputs
                    context->addOutputSynchro(featureCpl->getUuid(), synchronizer->toXmlString(false));

                    if (synchronizer->isSynchronized()) {

                        Poco::Path finalPath(featureCpl->getCplPath());
                        finalPath.setExtension("sync");
                        SyncFile* sync = synchronizer->generateSyncFile(finalPath.toString());
                        if (!sync->save()) {
                            Poco::Logger::get("SynchroThread").error("Sync generated could not be saved !", __FILE__, __LINE__);
                            continue;
                        }

                        // -- update sync on cpl
                        std::string sha1Sync = Converter::calculateSha1OfFile(finalPath.toString());


                        featureCpl->setCplInfos(featureCpl->getCplPath(), finalPath.toString(), sha1Sync, featureCpl->gettypeCpl());
                        Query* queryUpdate = featureCpl->updateQuery();
                        ResultQuery * result = context->executeQuery(queryUpdate);

                        if (!result->isValid()) {
                            Poco::Logger::get("SynchroThread").error("Update of Sync on cpl database failed !", __FILE__, __LINE__);
                        }

                        delete queryUpdate;
                        delete result;
                    }
                    else {
                        Poco::Logger::get("SynchroThread").error("Synchro failed : " + synchronizer->getOutputs(), __FILE__, __LINE__);
                    }

                    delete cutsFinal;
                    delete cplToSync;
                    delete lviRef;
                }

                // -- synchro by getting parent
                else {
                    Poco::Logger::get("SynchroThread").information("Script not present in release !", __FILE__, __LINE__);

                    std::shared_ptr<Release> parentRelease = release->getParentRelease();
                    if (parentRelease == nullptr) {
                        Poco::Logger::get("SynchroThread").warning("Parent release is not defined !", __FILE__, __LINE__);
                        break;
                    }
                    startScript = parentRelease->getFeatureScript();
                    
                    // -- level 1 synchro
                    if (startScript != nullptr) {

                        Poco::File pathScript(startScript->getPath());
                        std::vector<Poco::File> listFiles;
                        pathScript.list(listFiles);
                    
                        std::string pathlvi = "";
                        for (int i = 0; i < listFiles.size(); i++) {
                            Poco::Path pathFile(listFiles.at(i).path());
                            if (pathFile.getExtension() == "lvi") {
                                pathlvi = pathFile.toString();
                                break;
                            }
                        }

                        if (pathlvi.empty()) {
                            Poco::Logger::get("SynchroThread").error("Lvi of script not present !", __FILE__, __LINE__);
                            break;
                        }
                        LVI* lviRef = new LVI(pathlvi);
                        if (!lviRef->load()) {
                            Poco::Logger::get("SynchroThread").error("Could not load lvi file !", __FILE__, __LINE__);
                            break;
                        }

                        Poco::File xmlCpl(featureCpl->getCplPath());
                        if (!xmlCpl.exists()) {
                            Poco::Logger::get("SynchroThread").error("Cpl path does not exist !", __FILE__, __LINE__);
                            continue;
                        }

                        CplFile* cplToSync = new CplFile(featureCpl->getCplPath());
                        if (!cplToSync->load()) {
                            Poco::Logger::get("SynchroThread").error("Could not load cpl file !", __FILE__, __LINE__);
                            continue;
                        }

                        // -- prepare cuts
                        std::vector<std::shared_ptr<ReleaseCut>> cutsParent(parentRelease->getCuts());
                        if (cutsParent.size() > 0) {
                            Poco::Logger::get("SynchroThread").error("Parent release must not have cuts !", __FILE__, __LINE__);
                            break;
                        }

                        std::vector<std::shared_ptr<ReleaseCut>> cuts(release->getCuts());
                        CutScenes* cutsFinal = new CutScenes(cplToSync->getCplId(), cplToSync->getCplTitle(), lviRef->getCplId(), lviRef->getCplTitle());
                        for (std::shared_ptr<ReleaseCut> cut : cuts) {
                            cutsFinal->addCut(cut->getPosition(), cut->getPosition()+cut->getSize()-1, cut->getDescription());
                        }

                        // -- DO the synchro (only by duration : by id is done through manual synchro)
                        std::shared_ptr<Synchro> synchronizer = std::make_shared<Synchro>(Synchro(lviRef, cplToSync, cutsFinal));
                        synchronizer->synchronizeByDuration();

                        context->addOutputSynchro(featureCpl->getUuid(), synchronizer->toXmlString(false));

                        if (synchronizer->isSynchronized()) {

                            Poco::Path finalPath(featureCpl->getCplPath());
                            finalPath.setExtension("sync");
                            SyncFile* sync = synchronizer->generateSyncFile(finalPath.toString());
                            if (!sync->save()) {
                                Poco::Logger::get("SynchroThread").error("Sync generated could not be saved !", __FILE__, __LINE__);
                                continue;
                            }

                            // -- update sync on cpl
                            std::string sha1Sync = Converter::calculateSha1OfFile(finalPath.toString());

                            featureCpl->setCplInfos(featureCpl->getCplPath(), finalPath.toString(), sha1Sync, featureCpl->gettypeCpl());
                            Query* queryUpdate = featureCpl->updateQuery();
                            ResultQuery * result = context->executeQuery(queryUpdate);

                            if (!result->isValid()) {
                                Poco::Logger::get("SynchroThread").error("Update of Sync on cpl database failed !", __FILE__, __LINE__);
                            }

                            delete queryUpdate;
                            delete result;
                            
                        }
                        else {
                            Poco::Logger::get("SynchroThread").error("Synchro failed : " + synchronizer->getOutputs(), __FILE__, __LINE__);
                        }

                        delete cutsFinal;
                        delete cplToSync;
                        delete lviRef;
                    }

                    // -- level 2 synchro
                    else {

                        // -- check only one cpl in intermediate release (OV local) 
                        std::vector<std::shared_ptr<Cpl>> parentCpls = parentRelease->getFeatureCpls();
                        if (parentCpls.size() != 1) {
                            Poco::Logger::get("SynchroThread").error("Parent release has no CPL or more than one cpl associated !", __FILE__, __LINE__);
                            break;
                        }

                        std::shared_ptr<Cpl> parentCpl = parentCpls.at(0);
                        if (parentCpl->getSyncPath().empty()) {
                            Poco::Logger::get("SynchroThread").warning("Parent release has his CPL not synchronized !", __FILE__, __LINE__);
                            break;
                        }

                        SyncFile* syncRef = new SyncFile(parentCpl->getSyncPath());
                        if (!syncRef->load()) {
                            Poco::Logger::get("SynchroThread").warning("Parent release : sync file could not be loaded !", __FILE__, __LINE__);
                            break;
                        }

                        Poco::File xmlCpl(featureCpl->getCplPath());
                        if (!xmlCpl.exists()) {
                            Poco::Logger::get("SynchroThread").error("Cpl path does not exist !", __FILE__, __LINE__);
                            continue;
                        }

                        CplFile* cplToSync = new CplFile(featureCpl->getCplPath());
                        if (!cplToSync->load()) {
                            Poco::Logger::get("SynchroThread").error("Could not load cpl file !", __FILE__, __LINE__);
                            continue;
                        }

                        std::vector<std::shared_ptr<ReleaseCut>> cuts(release->getCuts());
                        CutScenes* cutsFinal = new CutScenes(cplToSync->getCplId(), cplToSync->getCplTitle(), syncRef->getCplId(), syncRef->getCplTitle());
                        for (std::shared_ptr<ReleaseCut> cut : cuts) {
                            cutsFinal->addCut(cut->getPosition(), cut->getPosition()+cut->getSize()-1, cut->getDescription());
                        }

                        std::shared_ptr<Synchro> synchronizer = std::make_shared<Synchro>(Synchro(syncRef, cplToSync, cutsFinal));
                        synchronizer->synchronizeByDuration();

                        context->addOutputSynchro(featureCpl->getUuid(), synchronizer->toXmlString(false));

                        if (synchronizer->isSynchronized()) {

                            Poco::Path finalPath(featureCpl->getCplPath());
                            finalPath.setExtension("sync");
                            SyncFile* sync = synchronizer->generateSyncFile(finalPath.toString());
                            if (!sync->save()) {
                                Poco::Logger::get("SynchroThread").error("Sync generated could not be saved !", __FILE__, __LINE__);
                                continue;
                            }

                            // -- update sync on cpl
                            std::string sha1Sync = Converter::calculateSha1OfFile(finalPath.toString());


                            featureCpl->setCplInfos(featureCpl->getCplPath(), finalPath.toString(), sha1Sync, featureCpl->gettypeCpl());
                            Query* queryUpdate = featureCpl->updateQuery();
                            ResultQuery * result = context->executeQuery(queryUpdate);

                            if (!result->isValid()) {
                                Poco::Logger::get("SynchroThread").error("Update of Sync on cpl database failed !", __FILE__, __LINE__);
                            }

                            delete queryUpdate;
                            delete result;

                        }
                        else {
                            Poco::Logger::get("SynchroThread").error("Synchro failed : " + synchronizer->getOutputs(), __FILE__, __LINE__);
                        }

                        delete cutsFinal;
                        delete cplToSync;
                        delete syncRef;
                    }
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