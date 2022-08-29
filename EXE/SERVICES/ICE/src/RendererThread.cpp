#include "RendererThread.h"

RendererThread::RendererThread()
{
    stop = false;
    thread = nullptr;
    ApplicationContext* context = ApplicationContext::getCurrentContext();
    int minThread = std::min(6, context->getConfig()->getIceConf()->getNbPanels() + 1);
    poolOfThread = new Poco::ThreadPool(minThread, 11, 1, 65536);
}
    
RendererThread::~RendererThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }

    poolOfThread->stopAll();
    delete poolOfThread;
}

void RendererThread::startThread()
{
    MovingHeadProjection* mgProj = ApplicationContext::getCurrentContext()->getMovingHeadProjector();
    if (mgProj == nullptr) {
        Poco::Logger::get("RendererThread").error("MovingHead projector is nullptr !");
        return;
    }

    // -- start thread
    thread = new Poco::Thread("Render");
    thread->setPriority(Poco::Thread::Priority::PRIO_HIGHEST);
    thread->start(*this);
}

void RendererThread::addCommand(std::shared_ptr<Command> cmd)
{
    while (!commandsMutex.tryLock()) {usleep(20);}
    commands.insert_or_assign(cmd->getUuid(), cmd);
    commandsMutex.unlock();
}

void RendererThread::run() {

    // -- work at 15ms (2 executions in a cycle of frame)
    int waitTime = 15;
    Poco::Stopwatch watch;
    Poco::Stopwatch watchDetail;
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    while (!stop) {

        //Poco::Logger::get("RendererThread").debug("Start loop render", __FILE__, __LINE__);
	    
        watch.reset();
        watch.start();

        //Poco::Logger::get("RendererThread").debug("Remove old outputs", __FILE__, __LINE__);
        context->removeOldOutput();

        // -- get current frame (recalculate at the time of execution)
        //Poco::Logger::get("RendererThread").debug("Read current frame", __FILE__, __LINE__);
        int currentFrame = context->getCurrentFrame();
        while (!context->tryLockScriptsMutex()) {usleep(20);}
        std::shared_ptr<LocalRepository::ScriptsInformations> script(context->getCurrentScript());
        std::shared_ptr<Cis> cis(context->getScriptDatas());
        context->unlockScriptsMutex();

        //Poco::Logger::get("RendererThread").debug("Check current frame : " + std::to_string(currentFrame), __FILE__, __LINE__);
        if (currentFrame != -1) {
            // -- check if already calculated : based on panels 
            // -- depth of 3 image template only
            int i = currentFrame, frameToCompute = -1;
            for (; i < currentFrame + 3; i++) {

                if (script != nullptr && cis != nullptr) {
                    int frameTest = i;
                    if (script->loop) {
                        frameTest = i % cis->getFrameCount();
                    }
                    //Poco::Logger::get("RendererThread").debug("Frame to test is exist : " + std::to_string(frameTest), __FILE__, __LINE__);
                    if (!context->hasFrameOutput(frameTest)) {
                        frameToCompute = frameTest;
                        //Poco::Logger::get("RendererThread").debug("Compute frame", __FILE__, __LINE__);
                        break;
                    }
                }
                else {
                    //Poco::Logger::get("RendererThread").debug("Cis or script is null", __FILE__, __LINE__);
                }
            }

            if (frameToCompute == -1) {
                // -- NOTHING TO DO
                // -- already computed max at the time
            }
            else {

                //Poco::Logger::get("RendererThread").debug("Current frame to render", __FILE__, __LINE__);
                if (cis == nullptr) {
                    Poco::Logger::get("RendererThread").error("ICE Script pointer is nullptr !", __FILE__, __LINE__);
                }
                else {
                    // -- we don't want to pollute queue with black datas : only if current frame
                    if (frameToCompute >= cis->getFrameCount()) {
                        if (frameToCompute == currentFrame) {
                            //Poco::Logger::get("RendererThread").debug("Paint black : cisNbFrame(" + std::to_string(cis->getFrameCount()) + "), frameToCompute (" + std::to_string(frameToCompute) + ")", __FILE__, __LINE__);
                        }
                    }
                    else {

                        //Poco::Logger::get("RendererThread").debug("Read frame image", __FILE__, __LINE__);
                        Poco::Stopwatch tempWatch;
                        tempWatch.start();
                        std::shared_ptr<Image> refImageTemplate = std::shared_ptr<Image>(cis->getRGBAImage(frameToCompute));
                        if (refImageTemplate == nullptr) {
                            Poco::Logger::get("RendererThread").error("Could not get image from cis file at index " + std::to_string(frameToCompute) + " !", __FILE__, __LINE__);
                        }
                        else {

                            Poco::Logger::get("RendererThread").information("Time to get image " + std::to_string(tempWatch.elapsed()), __FILE__, __LINE__);

                            // -- we start extraction
                            bool extractionSuccess = false;
                            Template* extractor = nullptr;

                            // -- set image for extraction
                            if (cis->getVersion() > 2) {
                                // -- prevent new version template
                            }
                            else {
                                extractor = new TemplateV1();
                                extractor->setImageTemplate(refImageTemplate);
                            }
                            
                            // -- define & start threads for extraction
                            ExtractorThread extractorMoving(ExtractorThread::MOVING_HEAD, extractor);
                            ExtractorThread extractorBackPar(ExtractorThread::BACK_PAR, extractor);
                            ExtractorThread extractorPanelBackLeft(ExtractorThread::PANEL_BACK_LEFT, extractor);
                            ExtractorThread extractorPanelBackRight(ExtractorThread::PANEL_BACK_RIGHT, extractor);
                            ExtractorThread extractorPanelForeLeft(ExtractorThread::PANEL_FORE_LEFT, extractor);
                            ExtractorThread extractorPanelForeRight(ExtractorThread::PANEL_FORE_RIGHT, extractor);

                            tempWatch.reset();
                            tempWatch.start();

                            //Poco::Logger::get("RendererThread").debug("Start extraction in template", __FILE__, __LINE__);
                            poolOfThread->start(extractorPanelBackLeft);
                            poolOfThread->start(extractorPanelBackRight);
                            poolOfThread->start(extractorPanelForeLeft);
                            poolOfThread->start(extractorPanelForeRight);
                            poolOfThread->start(extractorMoving);
                            poolOfThread->start(extractorBackPar);

                            // -- wait for extraction finished
                            poolOfThread->joinAll();
                            poolOfThread->collect();
                            Poco::Logger::get("RendererThread").information("Time to extract images " + std::to_string(tempWatch.elapsed()), __FILE__, __LINE__);
                            
                            //Poco::Logger::get("RendererThread").debug("Initialize output", __FILE__, __LINE__);
                            std::shared_ptr<FrameOutputs> outputs = std::make_shared<FrameOutputs>(FrameOutputs(frameToCompute));
                            context->addNewOutput(frameToCompute, outputs);
                            
                            //Poco::Logger::get("RendererThread").debug("Start projection", __FILE__, __LINE__);
                            tempWatch.reset();
                            tempWatch.start();
                            // -- projection on panels and moving heads
                            std::shared_ptr<MovingHeadProjectionThread> movingHeadThread = std::make_shared<MovingHeadProjectionThread>(MovingHeadProjectionThread(frameToCompute, extractor));
                            poolOfThread->start(*movingHeadThread.get());

                            // -- 1 thread by panel
                            std::vector<std::shared_ptr<PanelProjectionThread>> panelProjectors;
                            for (int i = 0; i < context->getConfig()->getIceConf()->getNbPanels(); i++) {

                                //Poco::Logger::get("RendererThread").debug("Create panel projection for :" + context->getConfig()->getIceConf()->getPanel(i)->name, __FILE__, __LINE__);
                                std::shared_ptr<PanelProjectionThread> projectorThread = std::make_shared<PanelProjectionThread>(context->getConfig()->getIceConf()->getPanel(i)->name, frameToCompute, extractor);
                                poolOfThread->start(*projectorThread.get());
                                panelProjectors.push_back(projectorThread);
                                projectorThread.reset();
                            }

                            // -- set PARS and BACKLIGHTs outputs
                            for (int i = 0; i < extractor->getNbBacklights(); i++) {
                                outputs->addBacklightOutput(extractor->getBacklightOutputs(i));
                            }
                            for (int i = 0; i < extractor->getNbPars(); i++) {
                                outputs->addParOutput(extractor->getParOutputs(i));
                            }

                            // -- wait for threads to finish
                            poolOfThread->joinAll();
                            poolOfThread->collect();
                            Poco::Logger::get("RendererThread").information("Time to project devices " + std::to_string(tempWatch.elapsed()), __FILE__, __LINE__);

                            // -- clean datas
                            //Poco::Logger::get("RendererThread").debug("Clean datas", __FILE__, __LINE__);
                            movingHeadThread.reset();
                            if (extractor != nullptr) {delete extractor;}
                            for (std::shared_ptr<PanelProjectionThread> projThread : panelProjectors) {
                                projThread.reset();
                            }
                            panelProjectors.clear();
                            outputs->setComplete();
                            outputs.reset();
                        }
                    
                        refImageTemplate.reset();
                    }
                }
            }
        }

        script.reset();
        cis.reset();

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

        // -- wait for next send request
        if (sleepTime > 0) {
            usleep(sleepTime);
        }
        else {
            Poco::Logger::get("RendererThread").warning("Thread exceeding time of " + std::to_string(-sleepTime) + "µs", __FILE__, __LINE__);
        }
    }
}

void RendererThread::playBlackOnAllDevices(int currentFrame)
{
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    std::map<std::string, Device *>::iterator itDevice;
    while (!context->tryLockListDevicesMutex()) {usleep(20);}
    std::map<std::string, Device *> listDevices(context->getListDevices());
    context->unlockListDevicesMutex();

    std::shared_ptr<FrameOutputs> outputs = std::make_shared<FrameOutputs>(FrameOutputs(currentFrame));
    context->addNewOutput(currentFrame, outputs);

    for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {
        
        if (itDevice->second->getType() == Device::LUMINOUS_TEXTIL) {

            LuminousTextile* panel = static_cast<LuminousTextile*>(itDevice->second);
            unsigned char* data = new unsigned char[panel->getWidth()*panel->getHeight()*3];
            for (int i = 0; i < panel->getWidth()*panel->getHeight()*3; i++) {
                data[i] = 0;
            }
            std::shared_ptr<OutputPanelImage> panelOutput = std::make_shared<OutputPanelImage>(OutputPanelImage(panel->getName(), std::make_shared<Image>(Image(panel->getWidth(), panel->getHeight(), PNG_FORMAT_RGB, data))));
            outputs->addPanelOutput(panelOutput);
        }

        else if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {
            DataEnabler* enabler = static_cast<DataEnabler*>(itDevice->second);
            for (int i = 0; i < enabler->getListDmxDevices().size(); i++) {
                
                if (enabler->getListDmxDevices().at(i)->getType() == DmxDevice::MOVING_HEAD) {
                    MovingHead* mh = static_cast<MovingHead*>(enabler->getListDmxDevices().at(i));
                    std::shared_ptr<MovingHeadProjection::OutputMovingHeadAngle> mhOutput = std::make_shared<MovingHeadProjection::OutputMovingHeadAngle>(MovingHeadProjection::OutputMovingHeadAngle());
                    mhOutput->name = mh->getName();
                    Color color;
                    color.setRGB(0, 0, 0);
                    mhOutput->color = color;
                    mhOutput->pan = 0;
                    mhOutput->tilt = 0;
                    outputs->addMovingHeadOutput(mhOutput);
                }
                else if (enabler->getListDmxDevices().at(i)->getType() == DmxDevice::PAR) {
                    PAR* par = static_cast<PAR*>(enabler->getListDmxDevices().at(i));
                    std::shared_ptr<Template::ParOutputs> parOutput = std::make_shared<Template::ParOutputs>(Template::ParOutputs());
                    parOutput->name = par->getName();
                    Color color;
                    color.setRGB(0, 0, 0);
                    parOutput->color = color;
                    outputs->addParOutput(parOutput);
                }
                else if (enabler->getListDmxDevices().at(i)->getType() == DmxDevice::BACKLIGHT) {
                    Backlight* bl = static_cast<Backlight*>(enabler->getListDmxDevices().at(i));
                    std::shared_ptr<Template::BacklightOutputs> blOutput = std::make_shared<Template::BacklightOutputs>(Template::BacklightOutputs());
                    blOutput->name = bl->getName();
                    Color color;
                    color.setRGB(0, 0, 0);
                    blOutput->color = color;
                    outputs->addBacklightOutput(blOutput);
                }
            }
        }
    }

    outputs->setComplete();
}

void RendererThread::executeCommand(std::shared_ptr<Command> cmd)
{
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    // -- response to fill even if error
    std::shared_ptr<CommandResponse> response = std::make_shared<CommandResponse>(CommandResponse(cmd->getUuid(), cmd->getType()));

    // -- PLAY TEMPLATE (only MANUAL)
    if (cmd->getType() == Command::PLAY_TEMPLATE_IMAGE) {
        handlePlayTemplateImage(cmd.get(), response.get());
    }

    else {
        Poco::Logger::get("RendererThread").warning("Player is not in MANUAL mode or command is not in force mode : impossible to execute command !", __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("Player is not in MANUAL mode or command is not in force mode : impossible to execute command !");
    }

    context->getCommandHandler()->addResponse(response);
}

void RendererThread::handlePlayTemplateImage(Command* cmd, CommandResponse* response)
{
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    std::shared_ptr<Image> imgPtr = cmd->getImageParameter();
    int version = cmd->getIntParameter("version", 1);
    std::string projectionStr = cmd->getStringParameter("projectionType", "UNKNOWN");
    int currentFrame = cmd->getIntParameter("frame", -1);

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
        projection = context->getProjectionType();
    }

    Template* extractor = nullptr;
    if (version > 2) {
        Poco::Logger::get("RendererThread").warning("Version more recent than V2 is not handled !", __FILE__, __LINE__);
        // -- TODO prevent new template version
    }
    else {
        extractor = new TemplateV1();
    }

    if (extractor != nullptr && currentFrame > -1 && imgPtr != nullptr) {

        // -- set image for extraction
        extractor->setImageTemplate(imgPtr);
        
        // -- define & start threads for extraction
        ExtractorThread extractorMoving(ExtractorThread::MOVING_HEAD, extractor);
        ExtractorThread extractorBackPar(ExtractorThread::BACK_PAR, extractor);
        ExtractorThread extractorPanelBackLeft(ExtractorThread::PANEL_BACK_LEFT, extractor);
        ExtractorThread extractorPanelBackRight(ExtractorThread::PANEL_BACK_RIGHT, extractor);
        ExtractorThread extractorPanelForeLeft(ExtractorThread::PANEL_FORE_LEFT, extractor);
        ExtractorThread extractorPanelForeRight(ExtractorThread::PANEL_FORE_RIGHT, extractor);

        poolOfThread->start(extractorMoving);
        poolOfThread->start(extractorBackPar);
        poolOfThread->start(extractorPanelBackLeft);
        poolOfThread->start(extractorPanelBackRight);
        poolOfThread->start(extractorPanelForeLeft);
        poolOfThread->start(extractorPanelForeRight);

        // -- wait for extraction finished
        poolOfThread->joinAll();
        poolOfThread->collect();

        std::shared_ptr<FrameOutputs> outputs = std::make_shared<FrameOutputs>(FrameOutputs(currentFrame));
        context->addNewOutput(currentFrame, outputs);
        
        // -- projection on panels and moving heads
        MovingHeadProjectionThread movingHeadThread(currentFrame, extractor);
        poolOfThread->start(movingHeadThread);

        // -- 1 thread by panel
        std::vector<std::shared_ptr<PanelProjectionThread>> panelProjectors;
        for (int i = 0; i < context->getConfig()->getIceConf()->getNbPanels(); i++) {
            std::shared_ptr<PanelProjectionThread> projectorThread = std::make_shared<PanelProjectionThread>(context->getConfig()->getIceConf()->getPanel(i)->name, currentFrame, extractor);
            poolOfThread->start(*projectorThread.get());
            panelProjectors.push_back(projectorThread);
            projectorThread.reset();
        }

        // -- set PARS and BACKLIGHTs outputs
        for (int i = 0; i < extractor->getNbBacklights(); i++) {
            outputs->addBacklightOutput(extractor->getBacklightOutputs(i));
        }
        for (int i = 0; i < extractor->getNbPars(); i++) {
            outputs->addParOutput(extractor->getParOutputs(i));
        }

        // -- wait for threads to finish
        poolOfThread->joinAll();
        poolOfThread->collect();

        // -- unlock mutexs and clear shared pointers to allow deletion
        for (std::shared_ptr<PanelProjectionThread> projThread : panelProjectors) {
            projThread.reset();
        }
        panelProjectors.clear();
        outputs->setComplete();
        outputs.reset();

        // -- force external current thread to be used by kinet thread to play image
        context->setCurrentExternalFrame(currentFrame);
    }
    else {
        // -- NOT HANDLED
    }

    if (extractor != nullptr) delete extractor;
    imgPtr.reset();
}