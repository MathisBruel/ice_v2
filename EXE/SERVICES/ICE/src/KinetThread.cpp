#include "KinetThread.h"

KinetThread::KinetThread()
{
    stop = false;
    thread = nullptr;

    int minThread = std::min(1, ApplicationContext::getCurrentContext()->getConfig()->getIceConf()->getNbPanels() + 1);
    poolOfThread = new Poco::ThreadPool(minThread, 11, 1, 32768);
}
    
KinetThread::~KinetThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
    poolOfThread->stopAll();
    delete poolOfThread;
}

void KinetThread::startThread()
{
    // -- check all is initialized before launch
    KinetMulticast* kinet = ApplicationContext::getCurrentContext()->getKinet();
    if (kinet == nullptr) {
        Poco::Logger::get("KinetThread").error("kinet communicator is nullptr !", __FILE__, __LINE__);
        return;
    }

    // -- start thread
    thread = new Poco::Thread("Kinet");
    thread->setPriority(Poco::Thread::Priority::PRIO_HIGHEST);
    thread->start(*this);
}

void KinetThread::addCommand(std::shared_ptr<Command> cmd)
{
    while (!commandsMutex.tryLock()) {usleep(20);}
    commands.insert_or_assign(cmd->getUuid(), cmd);
    commandsMutex.unlock();
}

void KinetThread::stopThread()
{
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    std::map<std::string, Device *>::iterator itDevice;
    while (!context->tryLockListDevicesMutex()) {usleep(20);}
    std::map<std::string, Device *> listDevices(context->getListDevices());
    context->unlockListDevicesMutex();

    for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {
        
        if (itDevice->second->getType() == Device::LUMINOUS_TEXTIL) {

            LuminousTextile* panel = static_cast<LuminousTextile*>(itDevice->second);
            std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::SET_COLOR));
            cmd->addParameter("device", panel->getName());
            cmd->addParameter("red", "0");
            cmd->addParameter("green", "0");
            cmd->addParameter("blue", "0");
            cmd->addParameter("force", "true");
            addCommand(cmd);
        }

        else if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {
            DataEnabler* enabler = static_cast<DataEnabler*>(itDevice->second);
            for (int i = 0; i < enabler->getListDmxDevices().size(); i++) {
                
                if (enabler->getListDmxDevices().at(i)->getType() == DmxDevice::MOVING_HEAD) {
                    MovingHead* mh = static_cast<MovingHead*>(enabler->getListDmxDevices().at(i));
                    std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::SET_COLOR));
                    cmd->addParameter("device", mh->getName());
                    cmd->addParameter("red", "0");
                    cmd->addParameter("green", "0");
                    cmd->addParameter("blue", "0");
                    cmd->addParameter("intensity", "0");
                    cmd->addParameter("white", "0");
                    cmd->addParameter("zoom", "0");
                    cmd->addParameter("pan", "0");
                    cmd->addParameter("tilt", "0");
                    cmd->addParameter("force", "true");
                    addCommand(cmd);
                }
                else if (enabler->getListDmxDevices().at(i)->getType() == DmxDevice::PAR) {
                    PAR* par = static_cast<PAR*>(enabler->getListDmxDevices().at(i));
                    std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::SET_COLOR));
                    cmd->addParameter("device", par->getName());
                    cmd->addParameter("red", "0");
                    cmd->addParameter("green", "0");
                    cmd->addParameter("blue", "0");
                    cmd->addParameter("intensity", "0");
                    cmd->addParameter("white", "0");
                    cmd->addParameter("zoom", "0");
                    cmd->addParameter("force", "true");
                    addCommand(cmd);
                }
                else if (enabler->getListDmxDevices().at(i)->getType() == DmxDevice::BACKLIGHT) {
                    Backlight* bl = static_cast<Backlight*>(enabler->getListDmxDevices().at(i));
                    std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::SET_COLOR));
                    cmd->addParameter("device", bl->getName());
                    cmd->addParameter("red", "0");
                    cmd->addParameter("green", "0");
                    cmd->addParameter("blue", "0");
                    cmd->addParameter("force", "true");
                    addCommand(cmd);
                }
            }
        }
    }

    usleep(5000000);

    stop = true;
}

void KinetThread::clearPlayoutThreads()
{
    for (PlayoutThread* threadPlayout : threadsPlayout) {
        delete threadPlayout;
    } 
    threadsPlayout.clear();
}

void KinetThread::run() {

    // -- work at 14ms
    int waitTime = 14;
    Poco::Stopwatch watch;
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    while (!stop) {
	    
        watch.reset();
        watch.start();

        // -- get current frame (recalculate at the time of execution)
        int currentFrame = context->getCurrentFrame();
        //Poco::Logger::get("KinetThread").debug("Get current frame : " + std::to_string(currentFrame), __FILE__, __LINE__);

        //Poco::Logger::get("KinetThread").debug("Get player state", __FILE__, __LINE__);
        while (!context->tryLockStatusPlayerMutex()) {usleep(20);}
        ApplicationContext::PlayerState state = context->getPlayerState();
        context->unlockStatusPlayerMutex();

        // -- detect if lut is to apply
        //Poco::Logger::get("KinetThread").debug("Get apply lut", __FILE__, __LINE__);
        bool lutApplied = true;
        if (state == ApplicationContext::EXTERNAL) {
            lutApplied = context->getStreamReceiver()->isLutApplied();
        }
        else {
            while (!context->tryLockScriptsMutex()) {usleep(20);}
            std::shared_ptr<Cis> cis = context->getScriptDatas();
            lutApplied = (cis == nullptr) ? false : cis->isLutApplied();
            context->unlockScriptsMutex();
            cis.reset();
        }
        
        bool hasDmxChange = false;

        // -- get datas from context
        //Poco::Logger::get("KinetThread").debug("Get devices", __FILE__, __LINE__);
        while (!context->tryLockListDevicesMutex()) {usleep(20);}
        std::map<std::string, Device*> listDevices(context->getListDevices());
        context->unlockListDevicesMutex();

        // -- remove potential older outputs
        std::shared_ptr<FrameOutputs> outputs = context->getOutputs(currentFrame);

        // -- parse panels
        if (outputs != nullptr) {

            if (!outputs->isComplete()) {
                //Poco::Logger::get("KinetThread").debug("Output is incomplete", __FILE__, __LINE__);
            } 
            else if (outputs->isAlreadyPlayed()) {
                //Poco::Logger::get("KinetThread").debug("Output is already played", __FILE__, __LINE__);
            }
            else {
                for (int i = 0; i < outputs->getNbPanelOutput(); i++) {
                    std::shared_ptr<OutputPanelImage> panelOutput(outputs->getPanelOutput(i));

                    // -- search for device
                    // -- do nothing if not found
                    std::map<std::string, Device *>::iterator itDevice = listDevices.find(panelOutput->getName());
                    if (itDevice == listDevices.end()) {
                        Poco::Logger::get("KinetThread").warning("Current device " + panelOutput->getName() + " is not found !", __FILE__, __LINE__);
                        panelOutput.reset();
                        continue;
                    }

                    // -- device is nullptr
                    Device* devicePanel = itDevice->second;
                    if (devicePanel == nullptr) {
                        Poco::Logger::get("KinetThread").warning("Current device " + panelOutput->getName() + " is nullptr !", __FILE__, __LINE__);
                        panelOutput.reset();
                        continue;
                    }

                    // -- ignored if device is alrady forced
                    if (context->isDeviceForced(devicePanel->getName())) {
                        //Poco::Logger::get("KinetThread").debug("Current device " + devicePanel->getName() + " is already forced : ignored !");
                        panelOutput.reset();
                        continue;
                    }
                    
                    // -- ignored if device is inactive (return of discovery)
                    if (!devicePanel->isActivate()) {
                        Poco::Logger::get("KinetThread").warning("Current device " + devicePanel->getName() + " is not active !", __FILE__, __LINE__);
                        panelOutput.reset();
                        continue;
                    }
                    
                    // -- found we create payload and we send new datas
                    if (devicePanel->getType() == Device::LUMINOUS_TEXTIL) {
                        LuminousTextile* panel = static_cast<LuminousTextile*>(devicePanel);
                        //Poco::Logger::get("KinetThread").debug("Create playout threads", __FILE__, __LINE__);
                        PlayoutThread* playout = new PlayoutThread(panel, panelOutput->getOut(), lutApplied);
                        threadsPlayout.push_back(playout);
                        poolOfThread->start(*playout);
                        //Poco::Logger::get("KinetThread").debug("Start playout threads", __FILE__, __LINE__);
                    }
                    panelOutput.reset();
                }
            
                // -- parse movingheads
                for (int i = 0; i < outputs->getNbMovingsOutput(); i++) {
                    std::shared_ptr<MovingHeadProjection::OutputMovingHeadAngle> mhOutput = outputs->getMovingHeadOutput(i);

                    // -- search for device dataEnabler where moving head is declared
                    bool dmxDeviceFound = false;
                    std::map<std::string, Device *>::iterator itDevice;
                    for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {
                        if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {

                            // -- ignored if device nullptr
                            if (itDevice->second == nullptr) {
                                Poco::Logger::get("KinetThread").warning("Current device is nullptr !");
                                continue;
                            }

                            // -- ignored if device is inactive (return of discovery)
                            if (!itDevice->second->isActivate()) {
                                Poco::Logger::get("KinetThread").warning("Current device " + itDevice->second->getName() + " is not active !");
                                continue;
                            }

                            DataEnabler* enabler = static_cast<DataEnabler*>(itDevice->second);
                            // -- parse list of dmx associated devices
                            for (int j = 0; j < enabler->getListDmxDevices().size(); j++) {
                                if (enabler->getListDmxDevices().at(j)->getName() == mhOutput->name) {

                                    dmxDeviceFound = true;

                                    // -- ignored if device is alrady forced
                                    if (context->isDeviceForced(mhOutput->name)) {
                                        //Poco::Logger::get("KinetThread").debug("Current device " + mhOutput->name + " is already forced : ignored !");
                                        break;
                                    }

                                    MovingHead* mh = static_cast<MovingHead*>(enabler->getListDmxDevices().at(j));
                                    double intensity = (mhOutput->color.getAlpha()*255.0+0.5)*mh->getRatioIntensity();
                                    mh->createPayload(mhOutput->color.getRed()*65535.0+0.5, mhOutput->color.getGreen()*65535.0+0.5, mhOutput->color.getBlue()*256.0*255.0+0.5,
                                                    mhOutput->color.getWhite()*65535.0+0.5, (uint8_t)intensity, mhOutput->pan, mhOutput->tilt, 
                                                    lutApplied);
                                    
                                    hasDmxChange = true;
                                    break;
                                }
                            }

                            if (dmxDeviceFound) {break;}
                        }
                    }
                
                    mhOutput.reset();
                }

                // -- parse pars
                for (int i = 0; i < outputs->getNbParOutput(); i++) {
                    std::shared_ptr<Template::ParOutputs> parOutput = outputs->getParOutput(i);
                
                    // -- search for device dataEnabler where moving head is declared
                    bool dmxDeviceFound = false;
                    std::map<std::string, Device *>::iterator itDevice;
                    for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {
                        if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {

                            // -- ignored if device nullptr
                            if (itDevice->second == nullptr) {
                                Poco::Logger::get("KinetThread").warning("Current device is nullptr !");
                                continue;
                            }

                            // -- ignored if device is inactive (return of discovery)
                            if (!itDevice->second->isActivate()) {
                                Poco::Logger::get("KinetThread").warning("Current device " + itDevice->second->getName() + " is not active !");
                                continue;
                            }

                            DataEnabler* enabler = static_cast<DataEnabler*>(itDevice->second);
                            // -- parse list of dmx associated devices
                            for (int j = 0; j < enabler->getListDmxDevices().size(); j++) {
                                if (enabler->getListDmxDevices().at(j)->getName() == parOutput->name) {

                                    dmxDeviceFound = true;

                                    // -- ignored if device is alrady forced
                                    if (context->isDeviceForced(parOutput->name)) {
                                        //Poco::Logger::get("KinetThread").debug("Current device " + parOutput->name + " is already forced : ignored !");
                                        break;
                                    }

                                    PAR* par = static_cast<PAR*>(enabler->getListDmxDevices().at(j));
                                    double intensity = (parOutput->color.getAlpha()*255.0+0.5)*par->getRatioIntensity();
                                    par->createPayload(parOutput->color.getRed()*65535.0+0.5, parOutput->color.getGreen()*65535.0+0.5, parOutput->color.getBlue()*256.0*255.0+0.5,
                                                    parOutput->color.getWhite()*65535.0+0.5, intensity, lutApplied);
                                    
                                    hasDmxChange = true;
                                    break;
                                }
                            }

                            if (dmxDeviceFound) {break;}
                        }
                    }
                
                    parOutput.reset();
                }

                // -- parse backlights
                for (int i = 0; i < outputs->getNbBacklightOutput(); i++) {
                    std::shared_ptr<Template::BacklightOutputs> backOutput = outputs->getBacklightOutput(i);
                
                    // -- search for device dataEnabler where moving head is declared
                    bool dmxDeviceFound = false;
                    std::map<std::string, Device *>::iterator itDevice;
                    for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {
                        if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {

                            // -- ignored if device nullptr
                            if (itDevice->second == nullptr) {
                                Poco::Logger::get("KinetThread").warning("Current device is nullptr !");
                                continue;
                            }

                            // -- ignored if device is inactive (return of discovery)
                            if (!itDevice->second->isActivate()) {
                                Poco::Logger::get("KinetThread").warning("Current device " + itDevice->second->getName() + " is not active !");
                                continue;
                            }

                            DataEnabler* enabler = static_cast<DataEnabler*>(itDevice->second);
                            // -- parse list of dmx associated devices
                            for (int j = 0; j < enabler->getListDmxDevices().size(); j++) {
                                if (enabler->getListDmxDevices().at(j)->getName() == backOutput->name) {

                                    dmxDeviceFound = true;

                                    // -- ignored if device is alrady forced
                                    if (context->isDeviceForced(backOutput->name)) {
                                        //Poco::Logger::get("KinetThread").debug("Current device " + backOutput->name + " is already forced : ignored !");
                                        break;
                                    }

                                    Backlight* backlight = static_cast<Backlight*>(enabler->getListDmxDevices().at(j));
                                            backlight->createPayload(backOutput->color.getRed()*255.0+0.5, backOutput->color.getGreen()*255.0+0.5, 
                                            backOutput->color.getBlue()*255.0+0.5, 0, lutApplied);
                                    hasDmxChange = true;
                                    break;
                                }
                            }

                            if (dmxDeviceFound) {break;}
                        }
                    }
                
                    backOutput.reset();
                }

                outputs->setPlayed();
                outputs.reset();
            }

        }

        // -- commands can be executed only if player is MANUAL (loop script not done at this point)
        int sleepTime = waitTime*1000-watch.elapsed();

        while (!commandsMutex.tryLock()) {usleep(20);}
        int sizeCommands = commands.size();
        commandsMutex.unlock();

        // -- execute command in time
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

        // -- play DMX buffer if not OFF STATE (SYNC, EXTERNAL AND MANUAL authorized)
        if (context->getPlayerState() != ApplicationContext::OFF) {
            std::map<std::string, Device *>::iterator itDevice;
            for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {
                if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {

                    if (itDevice->second == nullptr) {
                        Poco::Logger::get("KinetThread").warning("Current device " + itDevice->second->getName() + " is nullptr !");
                        continue;
                    }

                    if (!itDevice->second->isActivate()) {
                        Poco::Logger::get("KinetThread").warning("Current device " + itDevice->second->getName() + " is not active !");
                        continue;
                    }

                    DataEnabler* enabler = static_cast<DataEnabler*>(itDevice->second);
                    PlayoutThread* playout = new PlayoutThread(enabler, nullptr);
                    threadsPlayout.push_back(playout);
                    poolOfThread->start(*playout);
                }
            }
        }

        //Poco::Logger::get("KinetThread").debug("Clear threads", __FILE__, __LINE__);
        poolOfThread->joinAll();
        clearPlayoutThreads();
        poolOfThread->collect();

        sleepTime = waitTime*1000-watch.elapsed();

        // -- wait for next send request
        if (sleepTime > 0) {
            usleep(sleepTime);
        }
        else {
            Poco::Logger::get("KinetThread").warning("Thread exceeding time of " + std::to_string(-sleepTime) + "µs", __FILE__, __LINE__);
        }
    }
}

void KinetThread::executeCommand(std::shared_ptr<Command> cmd)
{
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    // -- response to fill even if error
    std::shared_ptr<CommandResponse> response = std::make_shared<CommandResponse>(CommandResponse(cmd->getUuid(), cmd->getType()));

    // -- mandatory datas from context
    KinetMulticast* kinet = context->getKinet();
    IceConfiguration* ice = context->getConfig()->getIceConf();
    while (!context->tryLockListDevicesMutex()) {usleep(20);}
    std::map<std::string, Device*> listDevices(context->getListDevices());
    context->unlockListDevicesMutex();

    while (!context->tryLockStatusPlayerMutex()) {usleep(20);}
    ApplicationContext::PlayerState state = context->getPlayerState();
    context->unlockStatusPlayerMutex();

    // -- no parameters : always forced
    if (cmd->getType() == Command::SET_PAR_SCENE_ON) {
        if (state == ApplicationContext::PlayerState::MANUAL || 
            state == ApplicationContext::PlayerState::SYNC) {

            std::map<std::string, Device *>::iterator itDevice;
            for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {
                if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {
                    DataEnabler* enabler = static_cast<DataEnabler*>(itDevice->second);

                    // -- parse Dmx devices of data enabler
                    for (int i = 0; i < enabler->getListDmxDevices().size(); i++) {
                        if (enabler->getListDmxDevices().at(i)->getType() == DmxDevice::PAR) {
                            PAR* par = static_cast<PAR*>(enabler->getListDmxDevices().at(i));

                            if (par->getCategory() == PAR::SCENE) {
                                par->createPayload(0, 0, 0, 65535, 255);
                                context->forceDevice(par->getName());
                            }
                        }
                    }

                    // -- update enabler buffer and send
                    PlayoutThread* playout = new PlayoutThread(enabler, nullptr);
                    threadsPlayout.push_back(playout);
                    poolOfThread->start(*playout);
                }
            }

            response->setStatus(CommandResponse::OK);
            response->setComments("Command SET_PAR_OFF achieved !");
        }
        else {
            response->setStatus(CommandResponse::KO);
            response->setComments("Command SET_PAR_OFF failed : mode is not manual or sync !");
        }
    }
    else if (cmd->getType() == Command::SET_PAR_SCENE_OFF) {
        if (state == ApplicationContext::PlayerState::MANUAL || 
            state == ApplicationContext::PlayerState::SYNC) {

            std::map<std::string, Device *>::iterator itDevice;
            for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {
                if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {
                    DataEnabler* enabler = static_cast<DataEnabler*>(itDevice->second);

                    // -- parse Dmx devices of data enabler
                    for (int i = 0; i < enabler->getListDmxDevices().size(); i++) {
                        if (enabler->getListDmxDevices().at(i)->getType() == DmxDevice::PAR) {
                            PAR* par = static_cast<PAR*>(enabler->getListDmxDevices().at(i));
                            
                            if (par->getCategory() == PAR::SCENE) {
                                par->createPayload(0, 0, 0, 0, 0);
                                context->forceDevice(par->getName());
                            }
                        }
                    }

                    // -- update enabler buffer and send
                    PlayoutThread* playout = new PlayoutThread(enabler, nullptr);
                    threadsPlayout.push_back(playout);
                    poolOfThread->start(*playout);
                }
            }

            response->setStatus(CommandResponse::OK);
            response->setComments("Command SET_PAR_OFF achieved !");
        }
        else {
            response->setStatus(CommandResponse::KO);
            response->setComments("Command SET_PAR_OFF failed : mode is not manual or sync !");
        }
    }
    else if (cmd->getType() == Command::SET_PAR_AMBIANCE_ON) {
        if (state == ApplicationContext::PlayerState::MANUAL || 
            state == ApplicationContext::PlayerState::SYNC) {

            std::map<std::string, Device *>::iterator itDevice;
            for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {
                if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {
                    DataEnabler* enabler = static_cast<DataEnabler*>(itDevice->second);

                    // -- parse Dmx devices of data enabler
                    for (int i = 0; i < enabler->getListDmxDevices().size(); i++) {
                        if (enabler->getListDmxDevices().at(i)->getType() == DmxDevice::PAR) {
                            PAR* par = static_cast<PAR*>(enabler->getListDmxDevices().at(i));

                            if (par->getCategory() == PAR::AMBIANCE) {
                                par->createPayload(0, 0, 0, 65535, 255);
                                context->forceDevice(par->getName());
                            }
                        }
                    }

                    // -- update enabler buffer and send
                    PlayoutThread* playout = new PlayoutThread(enabler, nullptr);
                    threadsPlayout.push_back(playout);
                    poolOfThread->start(*playout);
                }
            }

            response->setStatus(CommandResponse::OK);
            response->setComments("Command SET_PAR_OFF achieved !");
        }
        else {
            response->setStatus(CommandResponse::KO);
            response->setComments("Command SET_PAR_OFF failed : mode is not manual or sync !");
        }
    }
    else if (cmd->getType() == Command::SET_PAR_AMBIANCE_OFF) {
        if (state == ApplicationContext::PlayerState::MANUAL || 
            state == ApplicationContext::PlayerState::SYNC) {

            std::map<std::string, Device *>::iterator itDevice;
            for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {
                if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {
                    DataEnabler* enabler = static_cast<DataEnabler*>(itDevice->second);

                    // -- parse Dmx devices of data enabler
                    for (int i = 0; i < enabler->getListDmxDevices().size(); i++) {
                        if (enabler->getListDmxDevices().at(i)->getType() == DmxDevice::PAR) {
                            PAR* par = static_cast<PAR*>(enabler->getListDmxDevices().at(i));
                            
                            if (par->getCategory() == PAR::AMBIANCE) {
                                par->createPayload(0, 0, 0, 0, 0);
                                context->forceDevice(par->getName());
                            }
                        }
                    }

                    // -- update enabler buffer and send
                    PlayoutThread* playout = new PlayoutThread(enabler, nullptr);
                    threadsPlayout.push_back(playout);
                    poolOfThread->start(*playout);
                }
            }

            response->setStatus(CommandResponse::OK);
            response->setComments("Command SET_PAR_OFF achieved !");
        }
        else {
            response->setStatus(CommandResponse::KO);
            response->setComments("Command SET_PAR_OFF failed : mode is not manual or sync !");
        }
    }

    // -- need device to be specified
    else {
        // -- checking device
        std::string deviceId = cmd->getStringParameter("device");
        if (deviceId == "") {
            Poco::Logger::get("KinetThread").warning("Command KINET do not specify device concerned !");
            response->setStatus(CommandResponse::KO);
            response->setComments("Command KINET do not specify device concerned !");
        }

        else {

            // -- read all possible parameters
            bool force = cmd->getBoolParameter("force", false);
            bool lutToApply = cmd->getBoolParameter("lutToApply", false);

            // -- determine device concerned
            LuminousTextile* panel = nullptr;
            DmxDevice* dmx = nullptr;
            DataEnabler* enabler = nullptr;
            bool found = false;

            // -- search if device is a panel
            std::map<std::string, Device *>::iterator itDevice = listDevices.find(deviceId);
            if (itDevice != listDevices.end()) {
                if (itDevice->second->getType() == Device::LUMINOUS_TEXTIL && itDevice->second->isActivate()) {
                    panel = static_cast<LuminousTextile*>(itDevice->second);
                    found = true;
                }
                else {
                    Poco::Logger::get("KinetThread").warning("Command specify a device that is not a panel, surely a data enabler and it's not authorized !");
                }
            }

            // -- search if device is a DmxDevices
            else if (!found) {
                for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {
                    if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {
                        DataEnabler* enablerTemp = static_cast<DataEnabler*>(itDevice->second);

                        // -- parse Dmx devices of data enabler
                        for (int i = 0; i < enablerTemp->getListDmxDevices().size(); i++) {
                            if (enablerTemp->getListDmxDevices().at(i)->getName() == deviceId && enablerTemp->isActivate()) {
                                dmx = enablerTemp->getListDmxDevices().at(i);
                                enabler = enablerTemp;
                                found = true;
                                break;
                            }
                        }

                        if (found) {break;}
                    }
                }
            }

            // -- handle device not found
            if (!found) {
                Poco::Logger::get("KinetThread").warning("Device specified is not existing or cannot be changed (ex : data enabler) !");
                response->setStatus(CommandResponse::KO);
                response->setComments("Device specified is not existing or cannot be changed (ex : data enabler) !");
            }
            else if (force || (state == ApplicationContext::PlayerState::MANUAL && !context->isDeviceForced(deviceId))) {

                if (cmd->getType() == Command::SET_COLOR) {

                    // -- if panel : create a new image to create the payload
                    if (panel != nullptr) {

                        int red = cmd->getIntParameter("red", 0);
                        int green = cmd->getIntParameter("green", 0);
                        int blue = cmd->getIntParameter("blue", 0);

                        std::shared_ptr<Image> img = std::make_shared<Image>(panel->getWidth(), panel->getHeight(), PNG_FORMAT_RGB, red, green, blue);
                        if (force) {ApplicationContext::getCurrentContext()->forceDevice(deviceId);}

                        PlayoutThread* playout = new PlayoutThread(panel, img, lutToApply);
                        threadsPlayout.push_back(playout);
                        poolOfThread->start(*playout);

                        response->setStatus(CommandResponse::OK);
                        response->setComments("Color rgb send out to panel " + deviceId);
                    }

                    else {

                        // -- BACKLIGHT
                        if (dmx->getType() == DmxDevice::BACKLIGHT) {

                            int red = cmd->getIntParameter("red", 0);
                            int green = cmd->getIntParameter("green", 0);
                            int blue = cmd->getIntParameter("blue", 0);
                            int amber = cmd->getIntParameter("amber", 0);

                            Backlight* bl = static_cast<Backlight*>(dmx);
                            bl->createPayload(red, green, blue, amber, lutToApply);
                            response->setStatus(CommandResponse::OK);
                            response->setComments("Color rgb send out to backlight " + deviceId);
                        }
                        // -- PAR
                        else if (dmx->getType() == DmxDevice::PAR) {

                            PAR* par = static_cast<PAR*>(dmx);

                            double red = cmd->getDoubleParameter("red", 0);
                            double green = cmd->getDoubleParameter("green", 0);
                            double blue = cmd->getDoubleParameter("blue", 0);
                            double white = cmd->getDoubleParameter("white", 0);
                            int intensity = cmd->getIntParameter("intensity", 0);
                            int zoom = cmd->getIntParameter("zoom", par->getDefaultZoom());

                            par->createPayload(red*(65535.0/255.0), green*(65535.0/255.0), blue*(65535.0/255.0), white*(65535.0/255.0), intensity, zoom, lutToApply);
                            response->setStatus(CommandResponse::OK);
                            response->setComments("Color rgb, intensity and zoom send out to PAR " + deviceId);
                        }
                        // -- MOVING HEAD
                        else if (dmx->getType() == DmxDevice::MOVING_HEAD) {

                            MovingHead* mh = static_cast<MovingHead*>(dmx);

                            double red = cmd->getDoubleParameter("red", 0);
                            double green = cmd->getDoubleParameter("green", 0);
                            double blue = cmd->getDoubleParameter("blue", 0);
                            double white = cmd->getDoubleParameter("white", 0);
                            int intensity = cmd->getIntParameter("intensity", 0);
                            int zoom = cmd->getIntParameter("zoom", mh->getDefaultZoom());
                            double pan = cmd->getDoubleParameter("pan", 0);
                            double tilt = cmd->getDoubleParameter("tilt", 0);

                            mh->createPayload(red*(65535.0/255.0), green*(65535.0/255.0), blue*(65535.0/255.0), white*(65535.0/255.0), intensity, zoom, pan, tilt, lutToApply);
                            response->setStatus(CommandResponse::OK);
                            response->setComments("Color rgb, intensity, zoom, pan and tilt send out to moving head " + deviceId);
                        }

                        if (force) {ApplicationContext::getCurrentContext()->forceDevice(deviceId);}
                    }
                }

                // -- only for moving heads
                else if (cmd->getType() == Command::SET_POINTER) {
                    if (dmx != nullptr) {
                        if (dmx->getType() == DmxDevice::MOVING_HEAD) {

                            MovingHead* mh = static_cast<MovingHead*>(dmx);

                            double red = cmd->getDoubleParameter("red", 0);
                            double green = cmd->getDoubleParameter("green", 0);
                            double blue = cmd->getDoubleParameter("blue", 0);
                            double white = cmd->getDoubleParameter("white", 0);
                            int intensity = cmd->getIntParameter("intensity", 0);
                            int zoom = cmd->getIntParameter("zoom", mh->getDefaultZoom());
                            double ratioX = cmd->getDoubleParameter("x", 0.5);
                            double ratioY = cmd->getDoubleParameter("y", 0.5);

                            std::shared_ptr<MovingHeadProjection::OutputMovingHeadAngle> output = std::shared_ptr<MovingHeadProjection::OutputMovingHeadAngle>(MovingHeadProjection::projectPanTiltFromRatio(ice, deviceId, ratioX, ratioY));
                            if (output != nullptr) {
                                mh->createPayload(red*(65535.0/255.0), green*(65535.0/255.0), blue*(65535.0/255.0), white*(65535.0/255.0), intensity, zoom, output->pan, output->tilt, lutToApply);
                                if (force) {ApplicationContext::getCurrentContext()->forceDevice(deviceId);}

                                response->setStatus(CommandResponse::OK);
                                response->setComments("Cmd SET_POINTER achieved !");

                            }
                            else {
                                Poco::Logger::get("KinetThread").warning("Error happened while projecting moving head position !");
                                response->setStatus(CommandResponse::KO);
                                response->setComments("Error happened while projecting moving head position !");
                            }
                            output.reset();
                        }

                        else {
                            Poco::Logger::get("KinetThread").warning("Command SET_POINTER only apply to moving head devices !");
                            response->setStatus(CommandResponse::KO);
                            response->setComments("Command SET_POINTER only apply to moving head devices !");
                        }
                    }

                    // -- dmx devvice is not set
                    else {
                        Poco::Logger::get("KinetThread").warning("Command SET_POINTER only apply to moving head devices !");
                        response->setStatus(CommandResponse::KO);
                        response->setComments("Command SET_POINTER only apply to moving head devices !");
                    }
                }

            } 
            else {
                Poco::Logger::get("KinetThread").warning("Player is not in MANUAL mode or command is not in force mode : impossible to execute command !");
                response->setStatus(CommandResponse::KO);
                response->setComments("Player is not in MANUAL mode or command is not in force mode : impossible to execute command !");
            }
        }
    }
    context->getCommandHandler()->addResponse(response);
}