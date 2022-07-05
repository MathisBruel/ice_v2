#include "ContextThread.h"

ContextThread::ContextThread()
{
    stop = false;
    thread = nullptr;
}
    
ContextThread::~ContextThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}

void ContextThread::startThread()
{
    thread = new Poco::Thread("Context");
    thread->setPriority(Poco::Thread::Priority::PRIO_HIGH);
    thread->start(*this);
}

void ContextThread::addCommand(std::shared_ptr<Command> cmd)
{
    while (!commandsMutex.tryLock()) {usleep(20);}
    commands.insert_or_assign(cmd->getUuid(), cmd);
    commandsMutex.unlock();
}

void ContextThread::run() {

    Poco::Stopwatch watch;
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    // -- works at 14 ms rate
    int waitTime = 14;
    while (!stop) {
	    
        watch.reset();
        watch.start();
       
        context->handleSynchronization();
       
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

            std::shared_ptr<Command> cmd(it->second);
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
        }
        else {
            Poco::Logger::get("ContextThread").warning("Thread exceeding time of " + std::to_string(-sleepTime) + "µs", __FILE__, __LINE__);
        }
    }
}

void ContextThread::executeCommand(std::shared_ptr<Command> cmd)
{
    // -- create response to CMD
    //Poco::Logger::get("ContextThread").debug("Execute cmd type : " + std::to_string(cmd->getType()));
    std::shared_ptr<ContextResponse> response = std::make_shared<ContextResponse>(ContextResponse(cmd->getUuid(), cmd->getType()));
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    // -- only GET commands
    if (cmd->getType() == Command::STATUS_IMS) {
        response->setStatus(CommandResponse::OK);
        response->setComments("STATUS_IMS achieved");
        while (!context->tryLockStatusPlayerMutex()) {usleep(20);}
        response->setStatusPlayerIms(context->getPlayerStatus());
        context->unlockStatusPlayerMutex();
    }
    else if (cmd->getType() == Command::GET_CPL_INFOS) {
        response->setStatus(CommandResponse::OK);
        response->setComments("GET_CPL_INFOS achieved");
        while (!context->tryLockCplsMutex()) {usleep(20);}
        std::map<std::string, std::shared_ptr<CplInfos>> infos(context->getCplsInfos());
        response->setCplInfos(infos);
        context->unlockCplsMutex();
    }
    else if (cmd->getType() == Command::LIST_DEVICES) {

        response->setStatus(CommandResponse::OK);
        response->setComments("LIST_DEVICES achieved");
        
        while (!context->tryLockListDevicesMutex()) {usleep(20);}
        std::map<std::string, Device *> listDevices(context->getListDevices());
        context->unlockListDevicesMutex();
        while (!context->tryLockForcedDevicesMutex()) {usleep(20);}
        std::vector<std::string> forcedDevices(context->getForcedDevices());
        context->unlockForcedDevicesMutex();
        
        response->setListDevices(listDevices);
        response->setKinetConfiguration(context->getConfig()->getKinetConf());
        response->setForcedDevices(forcedDevices);
    }
    
    else if (cmd->getType() == Command::GET_DOWNLOADED_SCRIPTS) {
        response->setStatus(CommandResponse::OK);
        response->setComments("GET_DOWNLOADED_SCRIPTS achieved");
        while (!context->tryLockLocalRepoMutex()) {usleep(20);}
        std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>> pathToscripts(context->getRepoLocal()->getPathToScripts());
        context->unlockLocalRepoMutex();
        response->setDownloadedScripts(pathToscripts);
    }
    else if (cmd->getType() == Command::GET_MODE) {

        response->setStatus(CommandResponse::OK);
        response->setComments("GET_MODE achieved");
        while (!context->tryLockModeMutex()) {usleep(20);}
        response->setMode(context->getPlayerState());
        context->unlockModeMutex();
    }
    else if (cmd->getType() == Command::GET_CURRENT_SYNC) {
        response->setStatus(CommandResponse::OK);
        response->setComments("GET_CURRENT_SYNC achieved");
        while (!context->tryLockScriptsMutex()) {usleep(20);}
        std::shared_ptr<LocalRepository::ScriptsInformations> currentScript(context->getCurrentScript());
        std::shared_ptr<SyncFile> currentSync(context->getCurrentSync());
        std::shared_ptr<Cis> currentCis(context->getScriptDatas());
        context->unlockScriptsMutex();
        response->setCurrentSync(currentScript, currentSync, currentCis, context->getTimestampStartLoop());
    }
    else if (cmd->getType() == Command::GET_CURRENT_PROJECTION) {
        response->setStatus(CommandResponse::OK);
        response->setComments("GET_CURRENT_PROJECTION achieved");
        while (!context->tryLockProjectionMutex()) {usleep(20);}
        response->setProjection(context->getBaseProjectionType());
        context->unlockProjectionMutex();
    }
    else if (cmd->getType() == Command::GET_CONFIGURATION) {
        response->setStatus(CommandResponse::OK);
        response->setComments("GET_CONFIGURATION achieved");
        response->setIceConfiguration(context->getConfig()->getIceConf());
    }
    else if (cmd->getType() == Command::GET_SCENES) {
        response->setStatus(CommandResponse::OK);
        response->setComments("GET_SCENES achieved");
        response->setSceneConf(context->getConfig()->getSceneConf());
    }

    // -- actions to do
    else if (cmd->getType() == Command::STOP_SCRIPT) {
        handleStopScriptCmd(response.get());
    }
    else if (cmd->getType() == Command::SWITCH_PROJECTION) {
        while (!context->tryLockProjectionMutex()) {usleep(20);}
        handleSwitchProjectionCmd(cmd.get(), response.get());
        context->unlockProjectionMutex();
    }
    else if (cmd->getType() == Command::SWITCH_LUT) {
        handleSwitchLutCmd(cmd.get(), response.get());
    }
    else if (cmd->getType() == Command::UNFORCE_DEVICE) {
        handleUnforceCmd(cmd.get(), response.get());
    }
    else if (cmd->getType() == Command::UNFORCE_ALL_DEVICES) {
        handleUnforceAllCmd(response.get());
    }
    else if (cmd->getType() == Command::SWITCH_MODE) {
        handleSwitchModeCmd(cmd.get(), response.get());
    }
    else if (cmd->getType() == Command::REMOVE_SCENE) {
        std::string sceneName = cmd->getStringParameter("scene");
        if (!context->getConfig()->getSceneConf()->removeScene(sceneName)) {
            response->setStatus(CommandResponse::KO);
            response->setComments("REMOVE_SCENE error : no existing");
        }
        else {
            response->setStatus(CommandResponse::OK);
            response->setComments("REMOVE_SCENE achieved");
        }
    }
    else if (cmd->getType() == Command::CREATE_UPDATE_SCENE) {
        std::string sceneName = cmd->getStringParameter("scene");
        ComplexParam* sceneContent = cmd->getComplexParam("content");
        if (!context->getConfig()->getSceneConf()->addOrChangeScene(sceneName, sceneContent->toXmlString())) {
            response->setStatus(CommandResponse::KO);
            response->setComments("CREATE_UPDATE_SCENE error loading !");
        }
        else {
            response->setStatus(CommandResponse::OK);
            response->setComments("CREATE_UPDATE_SCENE achieved");
        }
    }
    else if (cmd->getType() == Command::PLAY_SCENE) {
        
        while (!context->tryLockModeMutex()) {usleep(20);}
        ApplicationContext::PlayerState state = context->getPlayerState();
        context->unlockModeMutex();

        while (!context->tryLockScriptsMutex()) {usleep(20);}
        std::shared_ptr<SyncFile> sync = context->getCurrentSync();
        context->unlockScriptsMutex();
        
        // -- ONLY if not playing something
        if (sync == nullptr && state == ApplicationContext::MANUAL) {
            std::string sceneName = cmd->getStringParameter("scene");
            SceneConfiguration::Scene* scene = context->getConfig()->getSceneConf()->getSceneByName(sceneName);

            if (scene != nullptr) {

                // -- launch command SET_COLOR Panels
                std::map<std::string, SceneConfiguration::ColorData *>::iterator itPanel;
                for (itPanel = scene->panels.begin(); itPanel != scene->panels.end(); itPanel++) {
                    std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::SET_COLOR));
                    cmd->addParameter("device", itPanel->first);
                    cmd->addParameter("red", std::to_string(itPanel->second->red));
                    cmd->addParameter("green", std::to_string(itPanel->second->green));
                    cmd->addParameter("blue", std::to_string(itPanel->second->blue));
                    cmd->addParameter("lutToApply", "true");
                    context->getCommandHandler()->addCommand(cmd);
                }

                // -- launch command SET_COLOR Backlights
                std::map<std::string, SceneConfiguration::ColorData *>::iterator itBack;
                for (itBack = scene->backlights.begin(); itBack != scene->backlights.end(); itBack++) {
                    std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::SET_COLOR));
                    cmd->addParameter("device", itBack->first);
                    cmd->addParameter("red", std::to_string(itBack->second->red));
                    cmd->addParameter("green", std::to_string(itBack->second->green));
                    cmd->addParameter("blue", std::to_string(itBack->second->blue));
                    cmd->addParameter("lutToApply", "true");
                    context->getCommandHandler()->addCommand(cmd);
                }

                // -- launch command SET_COLOR Pars
                std::map<std::string, SceneConfiguration::ParDatas *>::iterator itPar;
                for (itPar = scene->pars.begin(); itPar != scene->pars.end(); itPar++) {
                    std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::SET_COLOR));
                    cmd->addParameter("device", itPar->first);
                    cmd->addParameter("red", std::to_string(itPar->second->color.red));
                    cmd->addParameter("green", std::to_string(itPar->second->color.green));
                    cmd->addParameter("blue", std::to_string(itPar->second->color.blue));
                    cmd->addParameter("white", std::to_string(itPar->second->white));
                    cmd->addParameter("intensity", std::to_string(itPar->second->intensity));
                    cmd->addParameter("zoom", std::to_string(itPar->second->zoom));
                    cmd->addParameter("lutToApply", "true");
                    context->getCommandHandler()->addCommand(cmd);
                }

                // -- launch command SET_POINTER MovingHeads
                std::map<std::string, SceneConfiguration::MovingHeadDatas *>::iterator itMh;
                for (itMh = scene->movingheads.begin(); itMh != scene->movingheads.end(); itMh++) {
                    std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::SET_POINTER));
                    cmd->addParameter("device", itMh->first);
                    cmd->addParameter("red", std::to_string(itMh->second->color.red));
                    cmd->addParameter("green", std::to_string(itMh->second->color.green));
                    cmd->addParameter("blue", std::to_string(itMh->second->color.blue));
                    cmd->addParameter("white", std::to_string(itMh->second->white));
                    cmd->addParameter("intensity", std::to_string(itMh->second->intensity));
                    cmd->addParameter("zoom", std::to_string(itMh->second->zoom));
                    cmd->addParameter("ratioX", std::to_string(itMh->second->ratioX));
                    cmd->addParameter("ratioY", std::to_string(itMh->second->ratioY));
                    cmd->addParameter("lutToApply", "true");
                    context->getCommandHandler()->addCommand(cmd);
                }
            
                response->setStatus(CommandResponse::OK);
                response->setComments("PLAY_SCENE achieved !");
            }
            else {
                response->setStatus(CommandResponse::KO);
                response->setComments("PLAY_SCENE failed : scene do not exist !");
            }
        }
        else {
            response->setStatus(CommandResponse::KO);
            response->setComments("PLAY_SCENE failed : script playing or mode is not MANUAL !");
        }
    }

    // -- only in MANUAL
    else if (cmd->getType() == Command::LAUNCH_SCRIPT) {

        while (!context->tryLockModeMutex()) {usleep(20);}
        ApplicationContext::PlayerState state = context->getPlayerState();
        context->unlockModeMutex();

        bool forced = cmd->getBoolParameter("force", false);
        if (context->getPlayerState() == ApplicationContext::MANUAL || forced) {
            handleLaunchScriptCmd(cmd.get(), response.get());
        }
        else {
            response->setStatus(CommandResponse::KO);
            response->setComments("Can't launch script when mode is not in MANUAL !");
        }
    }

    context->getCommandHandler()->addResponse(response);
}

void ContextThread::handleStopScriptCmd(ContextResponse* response)
{
    if (ApplicationContext::getCurrentContext()->getPlayerState() == ApplicationContext::MANUAL || ApplicationContext::getCurrentContext()->isScriptForced()) {
        ApplicationContext::getCurrentContext()->stopCurrentScript();
        response->setStatus(CommandResponse::OK);
        response->setComments("STOP SCRIPT achieved !");
        return;
    }

    response->setStatus(CommandResponse::KO);
    response->setComments("STOP SCRIPT failed : Player not in MANUAL !");
}
void ContextThread::handleSwitchProjectionCmd(Command* cmd, ContextResponse* response)
{
    std::string projectionStr = cmd->getStringParameter("projection");
    PanelProjection::ProjectionType projection;
    if (projectionStr == "NO_FIT") {
        projection = PanelProjection::NO_FIT;
    }
    else if (projectionStr == "FIT_X") {
        projection = PanelProjection::FIT_X;
    }
    else if (projectionStr == "FIT_Y") {
        projection = PanelProjection::FIT_Y;
    }
    else if (projectionStr == "FIT_X_PROP_Y") {
        projection = PanelProjection::FIT_X_PROP_Y;
    }
    else if (projectionStr == "FIT_Y_PROP_X") {
        projection = PanelProjection::FIT_Y_PROP_X;
    }
    else if (projectionStr == "FIT_X_FIT_Y") {
        projection = PanelProjection::FIT_X_FIT_Y;
    }
    else {
        response->setStatus(CommandResponse::KO);
        response->setComments("Projection type " + projectionStr + " is not recognized !");
    }

    ApplicationContext::getCurrentContext()->setBaseProjectionType(projection);
    response->setStatus(CommandResponse::OK);
    response->setComments("SWITCH_PROJECTION achieved !");
}

void ContextThread::handleSwitchLutCmd(Command* cmd, ContextResponse* response)
{
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    // -- find concerned device
    std::string deviceId = cmd->getStringParameter("device");
    while (!context->tryLockListDevicesMutex()) {usleep(20);}
    std::map<std::string, Device *> listDevices(context->getListDevices());
    context->unlockListDevicesMutex();

    Device* panel = nullptr;
    DmxDevice* dmx = nullptr;
    Device* dmxController = nullptr;

    std::map<std::string, Device *>::iterator itDevice = listDevices.find(deviceId);

    // -- search for device
    if (itDevice == listDevices.end()) {

        bool found = false;
        // -- search in dmx devices
        for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {
            if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {
                DataEnabler* enablerTemp = static_cast<DataEnabler*>(itDevice->second);

                // -- parse Dmx devices of data enabler
                for (int i = 0; i < enablerTemp->getListDmxDevices().size(); i++) {
                    if (enablerTemp->getListDmxDevices().at(i)->getName() == deviceId) {
                        dmx = enablerTemp->getListDmxDevices().at(i);
                        dmxController = itDevice->second;
                        found = true;
                        break;
                    }
                }

                if (found) {break;}
            }
        }

        if (!found) {
            response->setStatus(CommandResponse::KO);
            response->setComments("Device " + deviceId + " not found !");
            return;
        }
    }
    else {
        panel = itDevice->second;
    }

    // -- treat for panels
    if (panel != nullptr) {

        // -- lut is already the right one
        std::string lutName = cmd->getStringParameter("lut");
        if (panel->getLutName() == lutName) {
            response->setStatus(CommandResponse::OK);
            response->setComments("No change : actual lut is already " + lutName);
        }

        // -- no lut assigned
        else if (lutName == "NONE") {
            panel->setColorCorrection("NONE", nullptr);
            response->setStatus(CommandResponse::OK);
            response->setComments("Device set with no LUT !");
        }

        // -- find if lut can be assigned to device
        else {
            LUT* lut = context->getConfig()->getLutConf()->getLutWithName(lutName);
            if (lut == nullptr) {
                response->setStatus(CommandResponse::KO);
                response->setComments("Lut " + lutName + " not found !");
                Poco::Logger::get("ContextThread").error("Lut " + lutName + " do not exist !", __FILE__, __LINE__);
            }
            else {
                KinetConfiguration::PanelConf* panelConf = context->getConfig()->getKinetConf()->getPanel(deviceId);
                if (panelConf == nullptr) {
                    response->setStatus(CommandResponse::KO);
                    response->setComments("Device " + deviceId + " not found in kinet configuration!");
                    Poco::Logger::get("ContextThread").error("No kinet conf for panel !", __FILE__, __LINE__);
                }
                else {
                    bool found = false;
                    for (int i = 0; i < panelConf->nbLut; i++) {
                        if (panelConf->lutPriority[i] == lutName) {
                            panel->setColorCorrection(lutName, lut);
                            response->setStatus(CommandResponse::OK);
                            response->setComments("SWITCH_LUT achieved !");
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        response->setStatus(CommandResponse::KO);
                        response->setComments("Lut " + lutName + " can not be assigned to device " + deviceId + " !");
                        Poco::Logger::get("ContextThread").error("LUT can't be assigned to panel !", __FILE__, __LINE__);
                    }
                }
            }
        }
    }

    else if (dmx != nullptr) {
        // -- lut is already the right one
        std::string lutName = cmd->getStringParameter("lut");
        if (dmx->getCalibName() == lutName) {
            response->setStatus(CommandResponse::OK);
            response->setComments("No change : actual lut is already " + lutName);
        }

        // -- no lut assigned
        else if (lutName == "NONE") {
            dmx->setColorCorrection("NONE", nullptr);
            response->setStatus(CommandResponse::OK);
            response->setComments("Device set with no LUT !");
        }

        // -- find if lut can be assigned to device
        else {
            Calibrer* calib = context->getConfig()->getLutConf()->getCalibWithName(lutName);
            if (calib == nullptr) {
                response->setStatus(CommandResponse::KO);
                response->setComments("Calib " + lutName + " not found !");
            }
            else {      

                // -- search conf for dmx controller associated
                KinetConfiguration::DmxControlerConf* dmxControllerConf = nullptr;
                for (int i = 0; i < context->getConfig()->getKinetConf()->getNbDmxControler(); i++) {
                    if (dmxController->getName() == context->getConfig()->getKinetConf()->getDmxControler(i)->name) {
                        dmxControllerConf = context->getConfig()->getKinetConf()->getDmxControler(i);
                        break;
                    }
                }

                if (dmxControllerConf == nullptr) {
                    response->setStatus(CommandResponse::KO);
                    response->setComments("Dmx controller not found in kinet configuration!");
                }
                else {

                    // -- search for conf of dmx device 
                    KinetConfiguration::DmxDeviceConf* dmxConf = nullptr;
                    for (int i = 0; i < dmxControllerConf->nbLights; i++) {
                        if (dmxControllerConf->lights[i].name == deviceId) {
                            dmxConf = &dmxControllerConf->lights[i];
                            break;
                        }
                    }

                    if (dmxConf == nullptr) {
                        response->setStatus(CommandResponse::KO);
                        response->setComments("Dmx not found in kinet configuration!");
                    }
                    else {

                        // -- search for lut name in list of lut of device
                        bool found = false;
                        for (int i = 0; i < dmxConf->nbCalib; i++) {
                            if (dmxConf->calib[i] == lutName) {
                                dmx->setColorCorrection(lutName, calib);
                                response->setStatus(CommandResponse::OK);
                                response->setComments("SWITCH_LUT achieved !");
                                found = true;
                                break;
                            }
                        }

                        if (!found) {
                            response->setStatus(CommandResponse::KO);
                            response->setComments("Lut " + lutName + " can not be assigned to device " + deviceId + " !");
                        }
                    }
                }
            }
        }
    }
    else {
        response->setStatus(CommandResponse::KO);
        response->setComments("No devices associated !");
    }
}   
void ContextThread::handleUnforceCmd(Command* cmd, ContextResponse* response)
{
    // -- find concerned device
    std::string deviceId = cmd->getStringParameter("device");
    ApplicationContext* context = ApplicationContext::getCurrentContext();
    
    context->unforceDevice(deviceId);
    response->setStatus(CommandResponse::OK);
    response->setComments("UNFORCE_DEVICE achieved !");
}   
void ContextThread::handleUnforceAllCmd(ContextResponse* response)
{
    ApplicationContext* context = ApplicationContext::getCurrentContext();
    context->unforceAllDevices();
    response->setStatus(CommandResponse::OK);
    response->setComments("UNFORCE_ALL_DEVICES achieved !");
}
void ContextThread::handleSwitchModeCmd(Command* cmd, ContextResponse* response)
{
    ApplicationContext* context = ApplicationContext::getCurrentContext();
    std::string mode = cmd->getStringParameter("mode");

    while (!context->tryLockModeMutex()) {usleep(20);}
    ApplicationContext::PlayerState state = context->getPlayerState();
    context->unlockModeMutex();

    if (mode == "OFF" && state != ApplicationContext::OFF) {

        if (state == ApplicationContext::EXTERNAL) {
            context->getStreamReceiver()->stopThread();
        }
        context->stopCurrentScript();
        context->playAllBlack(true);
        usleep(500000);
        context->unforceAllDevices();
        while (!context->tryLockModeMutex()) {usleep(20);}
        context->setPlayerState(ApplicationContext::OFF);
        context->unlockModeMutex();
        response->setStatus(CommandResponse::OK);
        response->setComments("Mode set to OFF !");
    }
    else if (mode == "MANUAL" && state != ApplicationContext::MANUAL) {

        if (state == ApplicationContext::EXTERNAL) {
            context->getStreamReceiver()->stopThread();
        }
        context->stopCurrentScript();
        while (!context->tryLockModeMutex()) {usleep(20);}
        context->setPlayerState(ApplicationContext::MANUAL);
        context->unlockModeMutex();
        response->setStatus(CommandResponse::OK);
        response->setComments("Mode set to MANUAL !");
    }
    else if (mode == "EXTERNAL" && state != ApplicationContext::EXTERNAL) {

        context->getStreamReceiver()->startThread();
        context->stopCurrentScript();
        while (!context->tryLockModeMutex()) {usleep(20);}
        context->setPlayerState(ApplicationContext::EXTERNAL);
        context->unlockModeMutex();
        response->setStatus(CommandResponse::OK);
        response->setComments("Mode set to EXTERNAL !");
    }
    else if (mode == "SYNC" && state != ApplicationContext::SYNC) {

        if (state == ApplicationContext::EXTERNAL) {
            context->getStreamReceiver()->stopThread();
        }
        while (!context->tryLockModeMutex()) {usleep(20);}
        context->setPlayerState(ApplicationContext::SYNC);
        context->unlockModeMutex();
        response->setStatus(CommandResponse::OK);
        response->setComments("Mode set to SYNC !");
    }
    else {
        response->setStatus(CommandResponse::KO);
        response->setComments("Mode " + mode + " not recognized !");
    }
}
void ContextThread::handleLaunchScriptCmd(Command* cmd, ContextResponse* response)
{
    std::string nameLcp = cmd->getStringParameter("script");
    ApplicationContext* context = ApplicationContext::getCurrentContext();
    if (nameLcp == "") {
        response->setStatus(CommandResponse::KO);
        response->setComments("Lcp not defined !");
    }
    else {
        std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>>::iterator itScripts;
        while (!context->tryLockLocalRepoMutex()) {usleep(20);}
        std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>> copyScriptsList(context->getRepoLocal()->getPathToScripts());
        context->unlockLocalRepoMutex();

        for (itScripts = copyScriptsList.begin(); itScripts != copyScriptsList.end() ; itScripts++) {
            
            if (itScripts->first.find(nameLcp) == std::string::npos) {
                response->setStatus(CommandResponse::KO);
                response->setComments("Script " + nameLcp + " not found !");
            }
            else {
                std::shared_ptr<LocalRepository::ScriptsInformations> scriptInfo(itScripts->second);

                if (!scriptInfo->playable) {
                    response->setStatus(CommandResponse::KO);
                    response->setComments("Script is not plyable !");
                    scriptInfo.reset();
                    return;
                }

                if (!scriptInfo->loop) {
                    response->setStatus(CommandResponse::KO);
                    response->setComments("Script is not loop : can't force launch of script that is not loop");
                    scriptInfo.reset();
                    return;
                }

                std::shared_ptr<Cis> cis = std::make_shared<Cis>(Cis());
                if (!cis->open(context->getRepoLocal()->getBasePath() + scriptInfo->infos.getNameScript() + "/" + scriptInfo->infos.getSha1Cis() + ".cis")) {
                    Poco::Logger::get("ContextThread").warning("Script datas " + scriptInfo->infos.getSha1Cis() + ".cis" + " can not be loaded !", __FILE__, __LINE__);
                    response->setStatus(CommandResponse::KO);
                    response->setComments("Script can't load cis file !");
                    scriptInfo.reset();
                    cis.reset();
                    return;
                }

                // -- we set start of loop
                // -- launch loop script is always forced
                while (!context->tryLockScriptsMutex()) {usleep(20);}
                context->setScriptDatas(cis);
                context->setCurrentScript(scriptInfo);
                context->setTimestampStartLoop(Poco::Timestamp().epochMicroseconds());
                context->setForceScript();
                scriptInfo->infos.setAlreadyPlayed();
                scriptInfo->infos.save(context->getRepoLocal()->getBasePath() + scriptInfo->infos.getNameScript() + "/INFOS");
                context->unlockScriptsMutex();
                Poco::Logger::get("ContextThread").information("Script " + nameLcp + " played forced", __FILE__, __LINE__);
                cis.reset();
                scriptInfo.reset();

                // -- we set response
                response->setStatus(CommandResponse::OK);
                response->setComments("Script loaded !");

                break;
            }
        }
    }
}