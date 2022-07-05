#include "ApplicationContext.h"

ApplicationContext* ApplicationContext::context = 0;

ApplicationContext* ApplicationContext::getCurrentContext()
{
    if (context == nullptr) {
        context = new ApplicationContext();
    }

    return context;
}

ApplicationContext::ApplicationContext()
{
    state = OFF;
    projectionType = PanelProjection::NO_FIT;
    scriptForce = false;
    timestampStartLoop = -1;
    PlayerStatus* status = new PlayerStatus();
    statusPlayer = std::shared_ptr<PlayerStatus>(status);
    repoCentral = nullptr;
}
ApplicationContext::~ApplicationContext()
{
    if (config != nullptr) {
        delete config;
    }

    if (ims != nullptr) {
        delete ims;
    }
    if (kinet != nullptr) {
        delete kinet;
    }

    std::map<std::string, Device *>::iterator itDevice;
    for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {
        if (itDevice->second != nullptr) {
            delete itDevice->second;
        }    
    }

    if (movingHead != nullptr) {
        delete movingHead;
    }
    if (repoCentral != nullptr) {
        delete repoCentral;
    }
    if (repoLocal != nullptr) {
        delete repoLocal;
    }

    if (commandHandler != nullptr) {
        delete commandHandler;
    }
    if (server != nullptr) {
        server->stop();
        delete server;
    }
}

bool ApplicationContext::initConfiguration(std::string pathConfigFiles)
{
    config = new Configuration(pathConfigFiles);
    return config->loadConfiguration();
}

bool ApplicationContext::initIms()
{
    IMSConfiguration* imsConf = config->getImsConf();

    ImsRequest::Protocol protocol = ImsRequest::Protocol::HTTP;
    ImsMessageHandler* msgHandler = nullptr;
    if (imsConf->getTypeIms() == "Doremi") {
        protocol = ImsRequest::Protocol::HTTP;
        msgHandler = new DoremiHandler();
    }
    else if (imsConf->getTypeIms() == "Barco") {
        protocol = ImsRequest::Protocol::HTTPS;
        msgHandler = new BarcoHandler();
    }
    else if (imsConf->getTypeIms() == "Christie") {
        protocol = ImsRequest::Protocol::UDP;
        msgHandler = new ChristieHandler();
    }
    else {
        Poco::Logger::get("ApplicationContext").error("Type of IMS not recognized in Ims configuration file !", __FILE__, __LINE__);
        return false;
    }

    Poco::Logger::get("ApplicationContext").debug("Opening IMS on " + imsConf->getIpIms() + " port " + std::to_string(imsConf->getPort()), __FILE__, __LINE__);
    ims = new ImsCommunicator(imsConf->getIpIms(), imsConf->getPort(), protocol);
    ims->addMessageHandler(msgHandler);
    return ims->open();
}

bool ApplicationContext::initKinet()
{
    kinet = new KinetMulticast("KinetInterface", config->getKinetConf()->getInterface());
    if (!kinet->open()) {
        Poco::Logger::get("ApplicationContext").error("Error when opening kinet sockets !", __FILE__, __LINE__);
        return false;
    }

    // -- fill list of panels
    for (int i = 0; i < config->getKinetConf()->getNbPanels() ; i++) {
        KinetConfiguration::PanelConf* panelConf = config->getKinetConf()->getPanel(i);

        LuminousTextile* panel = new LuminousTextile(panelConf->name, panelConf->name, 
                                                    std::to_string(panelConf->serial), panelConf->ip,
                                                    panelConf->nbPixels);

        // -- load 1st priority LUT
        if (panelConf->nbLut > 0) {
            std::string baseLut = panelConf->lutPriority[0];
            LUT* lut = config->getLutConf()->getLutWithName(baseLut);
            if (lut != nullptr) {
                panel->setColorCorrection(baseLut, lut);
            }
        }
        listDevices.insert_or_assign(panelConf->name, panel);
    }

    // -- fill list of dmx controllers && dmx devices associated
    for (int i = 0; i < config->getKinetConf()->getNbDmxControler() ; i++) {

        KinetConfiguration::DmxControlerConf* dmxConf = config->getKinetConf()->getDmxControler(i);
        DataEnabler* controler = new DataEnabler(dmxConf->name, dmxConf->name, std::to_string(dmxConf->serial), dmxConf->ip, 1);
        listDevices.insert_or_assign(dmxConf->name, controler);

        // -- instantiate all dmxs devices
        for (int j = 0; j < dmxConf->nbLights; j++) {

            DmxDevice* device = nullptr;
            KinetConfiguration::DmxDeviceConf dmxDeviceConf = dmxConf->lights[j];
            if (dmxDeviceConf.type == KinetConfiguration::BACKLIGHT) {
                device = new Backlight(dmxDeviceConf.name, dmxDeviceConf.name, 0x0000, dmxDeviceConf.dmxOffset, dmxDeviceConf.formatBuffer);
                controler->addDmxDevice(device);
            }
            else if (dmxDeviceConf.type == KinetConfiguration::PAR) {

                int lumens = dmxDeviceConf.lumens == 0 ? PAR::getDefaultLumens() : dmxDeviceConf.lumens;

                if (dmxDeviceConf.category == "FEATURE") {
                    device = new PAR(dmxDeviceConf.name, dmxDeviceConf.name, 0x0000, dmxDeviceConf.dmxOffset, dmxDeviceConf.formatBuffer, PAR::FEATURE, dmxDeviceConf.defaultZoom, lumens);
                }
                else if (dmxDeviceConf.category == "AMBIANCE") {
                    device = new PAR(dmxDeviceConf.name, dmxDeviceConf.name, 0x0000, dmxDeviceConf.dmxOffset, dmxDeviceConf.formatBuffer, PAR::AMBIANCE, dmxDeviceConf.defaultZoom, lumens);
                }
                else if (dmxDeviceConf.category == "SCENE") {
                    device = new PAR(dmxDeviceConf.name, dmxDeviceConf.name, 0x0000, dmxDeviceConf.dmxOffset, dmxDeviceConf.formatBuffer, PAR::SCENE, dmxDeviceConf.defaultZoom, lumens);
                }
                
                controler->addDmxDevice(device);
            }
            else if (dmxDeviceConf.type == KinetConfiguration::MOVINGHEAD) {
                IceConfiguration::MovingheadConf *confMH = context->getConfig()->getIceConf()->getMovingHead(dmxDeviceConf.name);
                int lumens = dmxDeviceConf.lumens == 0 ? MovingHead::getDefaultLumens() : dmxDeviceConf.lumens;
                device = new MovingHead(dmxDeviceConf.name, dmxDeviceConf.name, 0x0000, dmxDeviceConf.dmxOffset, dmxDeviceConf.formatBuffer, 
                confMH->panFullAngle, confMH->tiltFullAngle, confMH->biasPan, confMH->biasTilt, dmxDeviceConf.defaultZoom, lumens);
                controler->addDmxDevice(device);
            }
            else {
                Poco::Logger::get("ApplicationContext").warning("Device type not recognized !", __FILE__, __LINE__);
                continue;
            }

            // -- load 1st priority LUT
            if (dmxDeviceConf.nbCalib > 0) {
                std::string baseLut = dmxDeviceConf.calib[0];
                Calibrer* calib = config->getLutConf()->getCalibWithName(baseLut);
                if (calib != nullptr) {
                    device->setColorCorrection(baseLut, calib);
                }
            }

            if (!dmxDeviceConf.refCalib.empty()) {
                Calibrer* calib = config->getLutConf()->getCalibWithName(dmxDeviceConf.refCalib);
                if (calib != nullptr) {
                    device->setRefColorCorrection(calib);
                }
            }
        }
    }

    return true;
}

bool ApplicationContext::initRenderer()
{
    // -- need to know how is room is spatially placed
    movingHead = new MovingHeadProjection(config->getIceConf());
    return true;
}

bool ApplicationContext::initRepos()
{
    RepoConfiguration* repoConf = config->getRepoConf();
    repoCentral = new FtpConnector(repoConf->getHost(), repoConf->getPort(), repoConf->getUsername(), repoConf->getPassword());
    repoCentral->setDownloadPath(repoConf->getPathScripts());

    /*if (repoConf->getTypeServer() == RepoConfiguration::DOORWAY) {
        ((FtpConnector*)repoCentral)->addPathToCheck(FtpConnector::pathFtpScriptAmbiance);
    }
    else {

        if (repoConf->getRights() == RepoConfiguration::QC) {
            ((FtpConnector*)repoCentral)->addPathToCheck(FtpConnector::pathFtpScriptTest);
        }
        ((FtpConnector*)repoCentral)->addPathToCheck(FtpConnector::pathFtpScript);
        ((FtpConnector*)repoCentral)->addPathToCheck(FtpConnector::pathFtpScriptFilm);
    }*/
    ((FtpConnector*)repoCentral)->addPathToCheck("/CINEMAS/ICE/SCRIPT-ICE2");

    repoLocal = new LocalRepository(repoConf->getPathScripts());
    return true;
}

bool ApplicationContext::initHttpCommand()
{
    try {
        HttpCommandHandlerFactory* commandHandlerFactory = new HttpCommandHandlerFactory();
        server = new Poco::Net::HTTPServer(commandHandlerFactory, 8300, new Poco::Net::HTTPServerParams);
        server->start();

    } catch (std::exception &e) {
        Poco::Logger::get("ApplicationContext").error("Error at initialization of http command receiver !", __FILE__, __LINE__);
        return false;
    }

    return true;
}

bool ApplicationContext::initUdpCommand()
{
    try {
        IMSConfiguration* imsConf = config->getImsConf();
        Poco::Net::NetworkInterface interface = Poco::Net::NetworkInterface::forName(imsConf->getInterface());
        commandUdpReceiver = new UdpCommandReceiver(interface.address().toString(), 9011);
    
    } catch (std::exception &e) {
        Poco::Logger::get("ApplicationContext").error("Exception at initialization of the UDP command receiver !", __FILE__, __LINE__);
        return false;
    }

    return true;
}

bool ApplicationContext::initStreamReceiver()
{
    try {
        IMSConfiguration* imsConf = config->getImsConf();
        Poco::Net::NetworkInterface interface = Poco::Net::NetworkInterface::forName(imsConf->getInterface());
        streamer = new StreamReceiver(interface.address().toString(), 9015);
    
    } catch (std::exception &e) {
        Poco::Logger::get("ApplicationContext").error("Exception at initialization of the UDP command receiver !", __FILE__, __LINE__);
        return false;
    }

    return true;
}

void ApplicationContext::stopComponents()
{
    streamer->stopThread();
    Poco::Logger::get("ApplicationContext").information("Streamer stopped !", __FILE__, __LINE__);

    commandUdpReceiver->stopThread();
    Poco::Logger::get("ApplicationContext").information("CommandUdpReceiver stopped !", __FILE__, __LINE__);

    ims->close();
    Poco::Logger::get("ApplicationContext").information("Ims stopped !", __FILE__, __LINE__);

    repoCentral->close();
    Poco::Logger::get("ApplicationContext").information("RepoCentral stopped !", __FILE__, __LINE__);

    delete server;
    Poco::Logger::get("ApplicationContext").information("HttpServer stopped !", __FILE__, __LINE__);
}

void ApplicationContext::forceDevice(std::string name)
{
    bool found = false;
    for (int i = 0; i < forcedDevices.size(); i++) {
        if (forcedDevices.at(i) == name) {
            found = true;
            break;
        }
    }

    if (!found) {
        while (!tryLockForcedDevicesMutex()) {Timer::crossUsleep(20);}
        forcedDevices.push_back(name);
        unlockForcedDevicesMutex();
    }
}
void ApplicationContext::unforceDevice(std::string name)
{
    while (!tryLockForcedDevicesMutex()) {Timer::crossUsleep(20);}
    std::vector<std::string>::iterator it;
    for (it = forcedDevices.begin(); it != forcedDevices.end(); it++) {
        if (*it == name) {
            it = forcedDevices.erase(it);
            break;
        }
    }
    unlockForcedDevicesMutex();
}
void ApplicationContext::unforceAllDevices()
{
    while (!tryLockForcedDevicesMutex()) {Timer::crossUsleep(20);}
    forcedDevices.clear();
    unlockForcedDevicesMutex();
}

bool ApplicationContext::isDeviceForced(std::string name)
{
    while (!tryLockForcedDevicesMutex()) {Timer::crossUsleep(20);}
    for (int i = 0; i < forcedDevices.size(); i++) {
        if (forcedDevices.at(i) == name) {
            unlockForcedDevicesMutex();
            return true;
        }
    }
    unlockForcedDevicesMutex();
    return false;
}

PanelProjection::ProjectionType ApplicationContext::getProjectionType()
{
    // -- top level is the one defined in the sync file
    while(!tryLockScriptsMutex()) {Timer::crossUsleep(20);}
    std::shared_ptr<SyncFile> sync = currentSync;
    unlockScriptsMutex();

    if (sync != nullptr) {
        if (currentSync->getTypeProjection() != PanelProjection::UNKNOWN) {
            return currentSync->getTypeProjection();
        }
    }
    sync.reset();

    // -- otherwise the one in context is taken
    return projectionType;
}

void ApplicationContext::setPlayerStatus(std::shared_ptr<PlayerStatus> status) {
    while(!tryLockStatusPlayerMutex()) {Timer::crossUsleep(20);}
    statusPlayer = status;
    unlockStatusPlayerMutex();
}
void ApplicationContext::setCurrentScript(std::shared_ptr<LocalRepository::ScriptsInformations> currentScript) {
    while(!tryLockScriptsMutex()) {Timer::crossUsleep(20);}
    this->currentScript = currentScript;
    unlockScriptsMutex();
}
void ApplicationContext::setCurrentSync(std::shared_ptr<SyncFile> currentSync) 
{
    while(!tryLockScriptsMutex()) {Timer::crossUsleep(20);}
    this->currentSync = currentSync;
    unlockScriptsMutex();
}
void ApplicationContext::setScriptDatas(std::shared_ptr<Cis> scriptDatas) 
{
    while(!tryLockScriptsMutex()) {Timer::crossUsleep(20);}
    this->scriptDatas = scriptDatas;
    unlockScriptsMutex();
}

void ApplicationContext::stopCurrentScript()
{
    while (!tryLockScriptsMutex()) {Timer::crossUsleep(20);}
    currentScript.reset();
    currentSync.reset();
    scriptDatas.reset();
    timestampStartLoop = -1;
    scriptForce = false;
    unforceScript();
    unlockScriptsMutex();
}

void ApplicationContext::playAllBlack(bool forced)
{
    // -- search if device is a panel
    std::map<std::string, Device *>::iterator itDevice;
    while (!tryLockListDevicesMutex()) {Timer::crossUsleep(20);}
    std::map<std::string, Device *> listDevicesCopy(listDevices);
    unlockListDevicesMutex();

    for (itDevice = listDevicesCopy.begin(); itDevice != listDevicesCopy.end(); itDevice++) {
        if (itDevice->second->getType() == Device::LUMINOUS_TEXTIL) {
            std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::SET_COLOR));
            cmd->addParameter("device", itDevice->second->getName());
            cmd->addParameter("red", std::to_string(0));
            cmd->addParameter("green", std::to_string(0));
            cmd->addParameter("blue", std::to_string(0));
            if (forced) cmd->addParameter("force", "true");
            context->getCommandHandler()->addCommand(cmd);
        }
        else if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {
            DataEnabler* enablerTemp = static_cast<DataEnabler*>(itDevice->second);

            // -- parse Dmx devices of data enabler
            for (int i = 0; i < enablerTemp->getListDmxDevices().size(); i++) {
                DmxDevice *dmx = enablerTemp->getListDmxDevices().at(i);
                if (dmx->getType() == DmxDevice::BACKLIGHT) {
                    std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::SET_COLOR));
                    cmd->addParameter("device", dmx->getName());
                    cmd->addParameter("red", std::to_string(0));
                    cmd->addParameter("green", std::to_string(0));
                    cmd->addParameter("blue", std::to_string(0));
                    if (forced) cmd->addParameter("force", "true");
                    context->getCommandHandler()->addCommand(cmd);
                }
                else if (dmx->getType() == DmxDevice::PAR) {
                    std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::SET_COLOR));
                    cmd->addParameter("device", dmx->getName());
                    cmd->addParameter("red", std::to_string(0));
                    cmd->addParameter("green", std::to_string(0));
                    cmd->addParameter("blue", std::to_string(0));
                    cmd->addParameter("intensity", std::to_string(0));
                    cmd->addParameter("zoom", std::to_string(0));
                    if (forced) cmd->addParameter("force", "true");
                    context->getCommandHandler()->addCommand(cmd);
                }
                else if (dmx->getType() == DmxDevice::MOVING_HEAD) {
                    std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::SET_COLOR));
                    cmd->addParameter("device", dmx->getName());
                    cmd->addParameter("red", std::to_string(0));
                    cmd->addParameter("green", std::to_string(0));
                    cmd->addParameter("blue", std::to_string(0));
                    cmd->addParameter("intensity", std::to_string(0));
                    cmd->addParameter("zoom", std::to_string(0));
                    cmd->addParameter("pan", std::to_string(0));
                    cmd->addParameter("tilt", std::to_string(0));
                    if (forced) cmd->addParameter("force", "true");
                    context->getCommandHandler()->addCommand(cmd);
                }
            }
        }
    }
}

void ApplicationContext::insertPanelsOutput(int frameToCompute, std::shared_ptr<OutputPanelImage> output)
{
    while (!context->tryLockOutputsMutex()) {Timer::crossUsleep(20);}
    std::map<int, std::shared_ptr<FrameOutputs>>::iterator itFrameOutputs = outputs.find(frameToCompute);
    if (itFrameOutputs != outputs.end()) {
        itFrameOutputs->second->addPanelOutput(output);   
    }
    context->unlockOutputsMutex();
}
void ApplicationContext::insertBacklightOutput(int frameToCompute, std::shared_ptr<Template::BacklightOutputs> output)
{
    while (!context->tryLockOutputsMutex()) {Timer::crossUsleep(20);}
    std::map<int, std::shared_ptr<FrameOutputs>>::iterator itFrameOutputs = outputs.find(frameToCompute);
    if (itFrameOutputs != outputs.end()) {
        itFrameOutputs->second->addBacklightOutput(output);   
    }
    context->unlockOutputsMutex();
}
void ApplicationContext::insertParOutput(int frameToCompute, std::shared_ptr<Template::ParOutputs> output)
{
    while (!context->tryLockOutputsMutex()) {Timer::crossUsleep(20);}
    std::map<int, std::shared_ptr<FrameOutputs>>::iterator itFrameOutputs = outputs.find(frameToCompute);
    if (itFrameOutputs != outputs.end()) {
        itFrameOutputs->second->addParOutput(output);   
    }
    context->unlockOutputsMutex();
}
void ApplicationContext::insertMovingHeadOutput(int frameToCompute, std::shared_ptr<MovingHeadProjection::OutputMovingHeadAngle> output)
{
    while (!context->tryLockOutputsMutex()) {Timer::crossUsleep(20);}
    std::map<int, std::shared_ptr<FrameOutputs>>::iterator itFrameOutputs = outputs.find(frameToCompute);
    if (itFrameOutputs != outputs.end()) {
        itFrameOutputs->second->addMovingHeadOutput(output);   
    }
    context->unlockOutputsMutex();
}

void ApplicationContext::addNewOutput(int frameToCompute, std::shared_ptr<FrameOutputs> output)
{
    while (!context->tryLockOutputsMutex()) {Timer::crossUsleep(20);}
    outputs.insert_or_assign(frameToCompute, output);
    context->unlockOutputsMutex();
}
bool ApplicationContext::hasFrameOutput(int frameToCompute)
{
    bool found = false;
    while (!context->tryLockOutputsMutex()) {Timer::crossUsleep(20);}
    if (outputs.find(frameToCompute) != outputs.end()) {
        found = true;
    }
    context->unlockOutputsMutex();
    return found;
}
std::shared_ptr<FrameOutputs> ApplicationContext::getOutputs(int currentFrame)
{
    while (!context->tryLockOutputsMutex()) {Timer::crossUsleep(20);}
    std::map<int, std::shared_ptr<FrameOutputs>>::iterator itOutputs = outputs.find(currentFrame);
    context->unlockOutputsMutex();
    if (itOutputs != outputs.end()) {
        return itOutputs->second;
    }
    return nullptr;
}
void ApplicationContext::removeOldOutput()
{
    while (!context->tryLockOutputsMutex()) {Timer::crossUsleep(20);}
    std::map<int, std::shared_ptr<FrameOutputs>>::iterator it;
    //Poco::Logger::get("ApplicationContext").debug("Frame in outputs " + std::to_string(outputs.size()), __FILE__, __LINE__);
    for (it = outputs.begin(); it != outputs.end(); it++) {
        int64_t timestamp = Poco::Timestamp().epochMicroseconds();
        // -- more than 5 frames
        if (std::abs(timestamp - it->second->getTimestamp()) > 210000) {

            //Poco::Logger::get("ApplicationContext").debug("Frame timeout " + std::to_string(it->first), __FILE__, __LINE__);
            if (!it->second->isAlreadyPlayed()) {
                Poco::Logger::get("ApplicationContext").warning("Output missed for frame " + std::to_string(it->first), __FILE__, __LINE__);
            }
            if (it->second->isComplete()){

                it->second.reset();
                it = outputs.erase(it);
                if (it == outputs.end()) {
                    break;
                }
            }
        }
    }
    context->unlockOutputsMutex();
}

void ApplicationContext::addCPLInfo(std::shared_ptr<CplInfos> cplInfo)
{
    while (!cplsMutex.tryLock()) {Timer::crossUsleep(20);}
    cpls.insert_or_assign(cplInfo->getCplId(), cplInfo);
    cplsMutex.unlock();
}
void ApplicationContext::removeCPLInfo(std::string cplId)
{
    while (!cplsMutex.tryLock()) {Timer::crossUsleep(20);}
    cpls.erase(cplId);
    cplsMutex.unlock();
}
bool ApplicationContext::hasCplId(std::string cplId)
{
    bool found = false;
    while (!cplsMutex.tryLock()) {Timer::crossUsleep(20);}
    if (cpls.find(cplId) != cpls.end()) {
        found = true;
    }
    cplsMutex.unlock();
    return found;
}
std::shared_ptr<CplInfos> ApplicationContext::getCplsInfo(std::string cplId)
{
    std::shared_ptr<CplInfos> ptr;
    while (!cplsMutex.tryLock()) {Timer::crossUsleep(20);}
    std::map<std::string, std::shared_ptr<CplInfos>>::iterator it;
    it = cpls.find(cplId);
    if (it != cpls.end()) {
        ptr = it->second;
    }
    cplsMutex.unlock();
    return ptr;
}

void ApplicationContext::handleSynchronization()
{
    while (!tryLockModeMutex()) {Timer::crossUsleep(20);}
    ApplicationContext::PlayerState playerState = state;
    unlockModeMutex();

    // -- don't accept CMD neither loop or synchronization scripts
    if (playerState == OFF) {
        
    }

    // -- accept only streaming datas
    else if (playerState == EXTERNAL) {
        
    }

    else if (playerState == MANUAL) {
        // -- NOTHING TO DO
        // -- script and loop timestamp set at treatment of the command
    }

    // -- accept only synchronized scripts
    else if (playerState == SYNC) {

        while (!tryLockStatusPlayerMutex()) {Timer::crossUsleep(20);}
        std::shared_ptr<PlayerStatus> status(statusPlayer);
        unlockStatusPlayerMutex();

        while (!tryLockScriptsMutex()) {Timer::crossUsleep(20);}
        std::shared_ptr<LocalRepository::ScriptsInformations> script(currentScript);
        std::shared_ptr<SyncFile> syncF(currentSync);
        unlockScriptsMutex();

        // -- determine if curretn script needs to stop
        if (script.get() != nullptr && syncF != nullptr && !scriptForce) {

            if (syncF->getCplId() != status->getCplId()) {
                Poco::Logger::get("ApplicationContext").warning("Current script " + syncF->getCplId() + " is not corresponding to the CPL playing " + status->getCplId() + " : " + status->getCplTitle() + " !", __FILE__, __LINE__);

                // -- execute commands on end sync
                if (!syncF->getCommandsOnEndSync().empty()) {
                    for (int i = 0; i < syncF->getCommandsOnEndSync().size(); i++) {
                        commandHandler->addCommand(syncF->getCommandsOnEndSync().at(i));
                    }
                }

                stopCurrentScript();
            }
        }

        // -- only if playing or pause, others invalidate
        if (status->getState() == PlayerStatus::Play || status->getState() == PlayerStatus::Pause) {

            // -- if no script synchronized already, try to find script associated
            if (script.get() == nullptr) {

                while (!tryLockLocalRepoMutex()) {Timer::crossUsleep(20);}
                script = repoLocal->getScriptFromCplId(status->getCplId());
                unlockLocalRepoMutex();

                if (script.get() != nullptr) {
                    Poco::Path pathSyncFile(repoLocal->getBasePath() + script->infos.getNameScript());
                    pathSyncFile.setFileName(status->getCplId() + ".sync");
                    syncF = std::make_shared<SyncFile>(SyncFile(pathSyncFile.toString()));
                    if (!syncF->load()) {
                        Poco::Logger::get("ApplicationContext").warning("Script " + status->getCplId() + ".sync can not be loaded !", __FILE__, __LINE__);
                        syncF.reset();
                        return;
                    }
                    Poco::Path pathCisFile(repoLocal->getBasePath() + script->infos.getNameScript());
                    pathCisFile.setFileName(script->infos.getSha1Cis() + ".cis");
                    std::shared_ptr<Cis> cis = std::make_shared<Cis>(Cis());
                    if (!cis->open(pathCisFile.toString())) {
                        Poco::Logger::get("ApplicationContext").warning("Script datas " + pathCisFile.toString() + " can not be loaded !", __FILE__, __LINE__);
                        syncF.reset();
                        cis.reset();
                        return;
                    }

                    // -- execute commands on start sync
                    if (!syncF->getCommandsOnStartSync().empty()) {
                        for (int i = 0; i < syncF->getCommandsOnStartSync().size(); i++) {
                            commandHandler->addCommand(syncF->getCommandsOnStartSync().at(i));
                        }
                    }

                    while (!tryLockScriptsMutex()) {Timer::crossUsleep(20);}
                    setCurrentSync(syncF);
                    setScriptDatas(cis);
                    setCurrentScript(script);
                    if (script->loop) {
                        setTimestampStartLoop(Poco::Timestamp().epochMicroseconds());
                    }
                    script->infos.setAlreadyPlayed();
                    script->infos.save(repoLocal->getBasePath() + script->infos.getNameScript() + "/INFOS");
                    context->unlockScriptsMutex();
                    cis.reset();

                    Poco::Logger::get("ApplicationContext").information("Start playing script from synchronized CPL : " + script->infos.getNameScript(), __FILE__, __LINE__);
                }
                else {
                    Poco::Logger::get("ApplicationContext").warning("No synchronized script for current feature " + status->getCplTitle() + " !", __FILE__, __LINE__);
                }
            }
        }
    
        script.reset();
        status.reset();
        syncF.reset();
    }
}

int ApplicationContext::getCurrentFrame()
{
    while (!tryLockModeMutex()) {Timer::crossUsleep(20);}
    ApplicationContext::PlayerState playerState = state;
    unlockModeMutex();

    // -- don't accept CMD neither loop or synchronization scripts
    if (playerState == OFF) {return -1;}

    // -- accept command and loop scripts
    else if (playerState == MANUAL) {

        while (!tryLockScriptsMutex()) {Timer::crossUsleep(20);}
        std::shared_ptr<LocalRepository::ScriptsInformations> script(currentScript);
        std::shared_ptr<Cis> cis(scriptDatas);
        int64_t startLoop = timestampStartLoop;
        unlockScriptsMutex();

        if (script.get() != nullptr && cis.get() != nullptr) {

            // -- only loop necessite frame calculation here
            if (script->loop) {

                if (cis->getSpeedRate() != script->speedRate) {
                    Poco::Logger::get("ApplicationContext").error("Frame rate is different between scripts infos and lvi file !", __FILE__, __LINE__);
                    script.reset();
                    cis.reset();
                    return -1;
                }

                if (startLoop == -1) {
                    script.reset();
                    cis.reset();
                    Poco::Logger::get("ApplicationContext").error("A loop script is set but no start timestamp is given : should not happen !", __FILE__, __LINE__);
                    return -1;
                }

                int64_t diffTimeStamp = Poco::Timestamp().epochMicroseconds() - startLoop;
                int frame = (uint64_t)((double)diffTimeStamp/1000000 * cis->getSpeedRate() + 3.5) % cis->getFrameCount();
                return frame;
            }
            else
            {
                script.reset();
                cis.reset();
                // -- SHOULD NOT HAPPEN or no more synchronized scripts
                Poco::Logger::get("ApplicationContext").error("A no loop script is set but mode is MANUAL : should not happen !", __FILE__, __LINE__);
                return -1;
            }
        }
        // -- manual and no script setted
        else {
            script.reset();
            cis.reset();
            return -1;
        }
    }

    // -- accept only streaming datas
    else if (playerState == EXTERNAL) {
        int frame;
        while (!tryLockScriptsMutex()) {Timer::crossUsleep(20);}
        frame = currentExternalFrame;
        unlockScriptsMutex();
        return frame;
    }

    // -- accept only synchronized scripts
    else if (playerState == SYNC) {

        while (!tryLockStatusPlayerMutex()) {Timer::crossUsleep(20);}
        std::shared_ptr<PlayerStatus> status(statusPlayer);
        unlockStatusPlayerMutex();

        // -- only if playing or pause, others invalidate
        if ((status->getState() == PlayerStatus::Play || status->getState() == PlayerStatus::Pause) || scriptForce) {

            while (!tryLockScriptsMutex()) {Timer::crossUsleep(20);}
            std::shared_ptr<LocalRepository::ScriptsInformations> script(currentScript);
            std::shared_ptr<Cis> cis(scriptDatas);
            int startLoop = timestampStartLoop;
            std::shared_ptr<SyncFile> syncF(currentSync);
            unlockScriptsMutex();

            // -- check if script and sync
            if (script.get() == nullptr) {
                Poco::Logger::get("ApplicationContext").warning("Sync mode and player is playing feature but no script synchronized available !", __FILE__, __LINE__);
                status.reset();
                script.reset();
                cis.reset();
                syncF.reset();
                return -1;
            }

            // -- loop don't need to know timecode
            if (script->loop) {

                if (cis->getSpeedRate() != script->speedRate) {
                    Poco::Logger::get("ApplicationContext").error("Frame rate is different between scripts infos and lvi file !", __FILE__, __LINE__);
                    status.reset();
                    script.reset();
                    cis.reset();
                    syncF.reset();  
                    return -1;
                }

                if (startLoop == -1) {
                    Poco::Logger::get("ApplicationContext").error("A loop script is set but no start timestamp is given : should not happen !", __FILE__, __LINE__);
                    status.reset();
                    script.reset();
                    cis.reset();
                    syncF.reset();
                    return -1;
                }

                int64_t diffTimeStamp = Poco::Timestamp().epochMicroseconds() - startLoop;
                int frame = (uint64_t)((double)diffTimeStamp/1000000 * cis->getSpeedRate() + 0.5) % cis->getFrameCount();
                status.reset();
                script.reset();
                cis.reset();
                syncF.reset();
                return frame;
            }
            else
            {
                if (syncF.get() == nullptr) {
                    Poco::Logger::get("ApplicationContext").warning("Sync mode and player is playing feature but no script synchronized available !", __FILE__, __LINE__);
                    status.reset();
                    script.reset();
                    cis.reset();
                    syncF.reset();
                    return -1;
                }

                // -- determine if 2K or 4K to get bias for IMS 3000 only
                int offsetMs = 0;
                // -- only if playing otherwise ims slowly get closer to the real timecode over time
                if (status->getState() == PlayerStatus::Play) {
                    std::shared_ptr<CplInfos> info = getCplsInfo(status->getCplId());
                    if (info->getPictureWidth() > 3000) {
                        offsetMs = context->getConfig()->getImsConf()->getOffsetMs4K();
                    }
                    else {
                        offsetMs = context->getConfig()->getImsConf()->getOffsetMs2K();
                    }   
                }

                // -- calculate frame in script
                // -- take into accoutn entryPoint / startDelay and cutscenes
                double currentMsCpl = status->getTimeCodeMs() - offsetMs;

                // -- repercute time of treatment only if CPL is playing
                if (status->getState() == PlayerStatus::Play) {
                    currentMsCpl += (double)(Poco::Timestamp().epochMicroseconds() - status->getTimestamp())/1000.0;
                }

                int frame = syncF->getFrameFromCplDurationMs(currentMsCpl);
                status.reset();
                script.reset();
                cis.reset();
                syncF.reset();
                return frame;
            }
        }
        else {
            status.reset();
            return -1;
        }
    }
}