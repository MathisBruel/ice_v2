#include "ContextResponse.h"

ContextResponse::ContextResponse(std::string uuidFromCommand, Command::CommandType type)
    : CommandResponse(uuidFromCommand, type)
{

}
ContextResponse::~ContextResponse()
{
    statusPlayer.reset();
    std::map<std::string, std::shared_ptr<CplInfos>>::iterator itList;
    for (itList = listOfCpls.begin() ; itList != listOfCpls.end() ; itList++) {
        itList->second.reset();
    }
    listOfCpls.clear();

    std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>>::iterator itScript;
    for (itScript = pathToscripts.begin() ; itScript != pathToscripts.end() ; itScript++) {
        itScript->second.reset();
    }
    pathToscripts.clear();
    forcedDevices.clear();

    currentScript.reset();
    currentSync.reset();
    scriptDatas.reset();
}

void ContextResponse::setCurrentSync(std::shared_ptr<LocalRepository::ScriptsInformations> currentScript, std::shared_ptr<SyncFile> currentSync, std::shared_ptr<Cis> scriptDatas, uint64_t timestampStartLoop)
{
    this->currentScript = currentScript;
    this->currentSync = currentSync;
    this->scriptDatas = scriptDatas;
    this->timestampStartLoop = timestampStartLoop;
}

void ContextResponse::fillDatas()
{
    if (status == CommandResponse::OK) {

        Poco::AutoPtr<Poco::XML::Document> doc(new Poco::XML::Document);
        bool setXml = false;

        if (type == Command::CommandType::STATUS_IMS) {
            createPlayerStatusXml(doc);
            setXml = true;
        }
        else if (type == Command::CommandType::GET_CPL_INFOS) {
            createCplListXml(doc);
            setXml = true; 
        }
        else if (type == Command::CommandType::LIST_DEVICES) {
            createListDevicesXml(doc);
            setXml = true;
        }
        else if (type == Command::CommandType::GET_DOWNLOADED_SCRIPTS) {
            createDownloadedScriptsXml(doc);
            setXml = true;
        }
        else if (type == Command::CommandType::GET_MODE) {
            createModeXml(doc);
            setXml = true;
        }
        else if (type == Command::CommandType::GET_CURRENT_SYNC) {
            createCurrentSyncXml(doc);
            setXml = true;
        }
        else if (type == Command::CommandType::GET_CURRENT_PROJECTION) {
            createCurrentProjectionXml(doc);
            setXml = true;
        }
        else if (type == Command::CommandType::GET_CONFIGURATION) {
            createConfigurationXml(doc);
            setXml = true;
        }
        else if (type == Command::CommandType::GET_SCENES) {
            datas = sceneConf->getConfToStringXml();
        }

        // -- create string XML
        if (setXml) {
            std::ostringstream outStr;
            Poco::XML::DOMWriter builder;
            builder.setNewLine("\n");
            builder.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
            builder.writeNode(outStr, doc);
            datas = outStr.str();
        }

        doc->release();
    }
}
void ContextResponse::createPlayerStatusXml(Poco::XML::Document* doc)
{
    Poco::AutoPtr<Poco::XML::Element> root = doc->createElement("Player");
    doc->appendChild(root);

    // -- state
    Poco::AutoPtr<Poco::XML::Element> state = doc->createElement("State");
    Poco::AutoPtr<Poco::XML::Text> stateText;
    if (statusPlayer->getState() == PlayerStatus::Play) {
        stateText = doc->createTextNode("Play");
    } else if (statusPlayer->getState() == PlayerStatus::Pause) {
        stateText = doc->createTextNode("Pause");
    } else if (statusPlayer->getState() == PlayerStatus::Stop) {
        stateText = doc->createTextNode("Stop");
    } else {
        stateText = doc->createTextNode("Unknown");
    }
    state->appendChild(stateText);
    root->appendChild(state);

    // -- cplId
    Poco::AutoPtr<Poco::XML::Element> cplId = doc->createElement("CplId");
    Poco::AutoPtr<Poco::XML::Text> cplIdStr = doc->createTextNode(statusPlayer->getCplId());
    cplId->appendChild(cplIdStr);
    root->appendChild(cplId);

    // -- cplTitle
    Poco::AutoPtr<Poco::XML::Element> cplTitle = doc->createElement("CplTitle");
    Poco::AutoPtr<Poco::XML::Text> cplTitleStr = doc->createTextNode(statusPlayer->getCplTitle());
    cplTitle->appendChild(cplTitleStr);
    root->appendChild(cplTitle);

    // -- timecode
    Poco::AutoPtr<Poco::XML::Element> timecode = doc->createElement("Timecode");
    double timeCodems = statusPlayer->getTimeCodeMs();

    std::string timecodeS = "00:00:00:00";

    if (statusPlayer->getCplId() != "00000000-0000-0000-0000-000000000000") {
        int hour = timeCodems / 3600000;
        timeCodems -= hour * 3600000;
        int minutes = timeCodems / 60000;
        timeCodems -= minutes * 60000;
        int secondes = timeCodems / 1000;
        timeCodems -= secondes * 1000;
        int frame = ((timeCodems/1000.0) * statusPlayer->getSpeedRate())+0.5;
        
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << hour << ":" << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << secondes << ":" << std::setw(2) << std::setfill('0') << frame;
        timecodeS = ss.str();
    }

    Poco::AutoPtr<Poco::XML::Text> timecodeStr = doc->createTextNode(timecodeS);
    timecode->appendChild(timecodeStr);
    root->appendChild(timecode);
}
void ContextResponse::createCplListXml(Poco::XML::Document* doc)
{
    Poco::AutoPtr<Poco::XML::Element> root = doc->createElement("CPLS");
    doc->appendChild(root);

    // -- fill list
    std::map<std::string, std::shared_ptr<CplInfos>>::iterator it;
    for (it = listOfCpls.begin(); it != listOfCpls.end(); it++) {
        if (it->second != nullptr) {
            Poco::AutoPtr<Poco::XML::Element> cpl = doc->createElement("CPL");
            cpl->setAttribute("id", it->second->getCplId());

            // -- title
            Poco::AutoPtr<Poco::XML::Element> title = doc->createElement("title");
            Poco::AutoPtr<Poco::XML::Text> titleText = doc->createTextNode(it->second->getCplTitle());
            title->appendChild(titleText);
            cpl->appendChild(title);

            // -- type
            Poco::AutoPtr<Poco::XML::Element> type = doc->createElement("type");
            Poco::AutoPtr<Poco::XML::Text> typeText = doc->createTextNode(CplFile::cplTypeToString(it->second->getCplType()));
            type->appendChild(typeText);
            cpl->appendChild(type);

            // -- content version
            Poco::AutoPtr<Poco::XML::Element> contentVersion = doc->createElement("contentVersion");
            Poco::AutoPtr<Poco::XML::Text> contentVersionText = doc->createTextNode(it->second->getcontentVersionId());
            contentVersion->appendChild(contentVersionText);
            cpl->appendChild(contentVersion);

            // -- playable
            Poco::AutoPtr<Poco::XML::Element> playable = doc->createElement("playable");
            Poco::AutoPtr<Poco::XML::Text> playableText = doc->createTextNode(it->second->isPlayable() ? "true" : "false");
            playable->appendChild(playableText);
            cpl->appendChild(playable);

            // -- 3D
            Poco::AutoPtr<Poco::XML::Element> threeD = doc->createElement("ThreeD");
            Poco::AutoPtr<Poco::XML::Text> threeDText = doc->createTextNode(it->second->has3D() ? "true" : "false");
            threeD->appendChild(threeDText);
            cpl->appendChild(threeD);

            // -- speedRate
            Poco::AutoPtr<Poco::XML::Element> speedRate = doc->createElement("speedRate");
            Poco::AutoPtr<Poco::XML::Text> speedRateText = doc->createTextNode(std::to_string(it->second->getSpeedRate()));
            speedRate->appendChild(speedRateText);
            cpl->appendChild(speedRate);

            // -- cplDuration
            Poco::AutoPtr<Poco::XML::Element> cplDuration = doc->createElement("cplDuration");
            Poco::AutoPtr<Poco::XML::Text> cplDurationText = doc->createTextNode(std::to_string(it->second->getCplDuration()));
            cplDuration->appendChild(cplDurationText);
            cpl->appendChild(cplDuration);
            root->appendChild(cpl);
        }
    }
}
void ContextResponse::createListDevicesXml(Poco::XML::Document* doc)
{
    Poco::AutoPtr<Poco::XML::Element> root = doc->createElement("Devices");
    doc->appendChild(root);

    std::map<std::string, Device*>::iterator itDevice;
    for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++) {

        Poco::AutoPtr<Poco::XML::Element> device = doc->createElement("Device");
        
        // -- name
        Poco::AutoPtr<Poco::XML::Element> name = doc->createElement("Name");
        Poco::AutoPtr<Poco::XML::Text> nameStr = doc->createTextNode(itDevice->first);
        name->appendChild(nameStr);
        device->appendChild(name);

        // -- type
        Poco::AutoPtr<Poco::XML::Element> type = doc->createElement("Type");
        std::string typeString = "";
        if (itDevice->second->getType() == Device::LUMINOUS_TEXTIL) {
            typeString = "LUMINOUS_TEXTIL";
        }
        else if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {
            typeString = "DATA_ENABLER_PRO";
        }
        else {
            typeString = "UNKNOWN";
        }
        Poco::AutoPtr<Poco::XML::Text> typeStr = doc->createTextNode(typeString);
        type->appendChild(typeStr);
        device->appendChild(type);

        // -- ip 
        Poco::AutoPtr<Poco::XML::Element> ip = doc->createElement("Ip");
        Poco::AutoPtr<Poco::XML::Text> ipStr = doc->createTextNode(itDevice->second->getIp());
        ip->appendChild(ipStr);
        device->appendChild(ip);

        // -- ports
        Poco::AutoPtr<Poco::XML::Element> ports = doc->createElement("Ports");
        Poco::AutoPtr<Poco::XML::Text> portsStr = doc->createTextNode(std::to_string(itDevice->second->getNbPorts()));
        ports->appendChild(portsStr);
        device->appendChild(ports);

        // -- active
        Poco::AutoPtr<Poco::XML::Element> active = doc->createElement("Active");
        Poco::AutoPtr<Poco::XML::Text> activeStr = doc->createTextNode(itDevice->second->isActivate() ? "true" : "false");
        active->appendChild(activeStr);
        device->appendChild(active);

        if (itDevice->second->getType() == Device::LUMINOUS_TEXTIL) {
            LuminousTextile* panel = static_cast<LuminousTextile*>(itDevice->second);
            
            // -- witdh 
            Poco::AutoPtr<Poco::XML::Element> witdh = doc->createElement("Width");
            Poco::AutoPtr<Poco::XML::Text> witdhStr = doc->createTextNode(std::to_string(panel->getWidth()));
            witdh->appendChild(witdhStr);
            device->appendChild(witdh);

            // -- height 
            Poco::AutoPtr<Poco::XML::Element> height = doc->createElement("Height");
            Poco::AutoPtr<Poco::XML::Text> heightStr = doc->createTextNode(std::to_string(panel->getHeight()));
            height->appendChild(heightStr);
            device->appendChild(height);

            // -- forced
            Poco::AutoPtr<Poco::XML::Element> forcedTag = doc->createElement("Forced");
            bool forced = false;
            for (int j = 0; j < forcedDevices.size(); j++) {
                if (forcedDevices.at(j) == panel->getName()) {
                    forced = true;
                    break;
                }
            }
            Poco::AutoPtr<Poco::XML::Text> forcedStr = doc->createTextNode(forced ? "true" : "false");
            forcedTag->appendChild(forcedStr);
            device->appendChild(forcedTag);

            // -- lut 
            Poco::AutoPtr<Poco::XML::Element> currentLut = doc->createElement("LUT");
            Poco::AutoPtr<Poco::XML::Text> currentLutStr = doc->createTextNode(panel->getLutName());
            currentLut->appendChild(currentLutStr);
            device->appendChild(currentLut);

            // -- luts 
            KinetConfiguration::PanelConf* panelConf = kinet->getPanel(panel->getName());
            Poco::AutoPtr<Poco::XML::Element> luts = doc->createElement("LUTS");
            for (int i = 0; i < panelConf->nbLut; i++) {
                Poco::AutoPtr<Poco::XML::Element> lut = doc->createElement("LUT");
                Poco::AutoPtr<Poco::XML::Text> lutStr = doc->createTextNode(panelConf->lutPriority[i]);
                lut->appendChild(lutStr);
                luts->appendChild(lut);
            }
            device->appendChild(luts);
        }
        else if (itDevice->second->getType() == Device::DATA_ENABLER_PRO) {
            Poco::AutoPtr<Poco::XML::Element> dmxs = doc->createElement("DMXS");

            DataEnabler* enabler = static_cast<DataEnabler*>(itDevice->second);
            KinetConfiguration::DmxControlerConf* dmxControllerConf = kinet->getDmxControler(enabler->getName());
            
            // -- fill list of dmxs
            for (int i = 0; i < enabler->getListDmxDevices().size(); i++) {

                Poco::AutoPtr<Poco::XML::Element> dmx = doc->createElement("DMX");
                // -- name
                Poco::AutoPtr<Poco::XML::Element> nameDmx = doc->createElement("Name");
                Poco::AutoPtr<Poco::XML::Text> nameDmxStr = doc->createTextNode(enabler->getListDmxDevices().at(i)->getName());
                nameDmx->appendChild(nameDmxStr);
                dmx->appendChild(nameDmx);

                // -- offset
                Poco::AutoPtr<Poco::XML::Element> offset = doc->createElement("Offset");
                Poco::AutoPtr<Poco::XML::Text> offsetStr = doc->createTextNode(std::to_string(enabler->getListDmxDevices().at(i)->getDmxOffset()));
                offset->appendChild(offsetStr);
                dmx->appendChild(offset);

                // -- template
                Poco::AutoPtr<Poco::XML::Element> buffer = doc->createElement("Template");
                Poco::AutoPtr<Poco::XML::Text> templateStr = doc->createTextNode(enabler->getListDmxDevices().at(i)->getBufferTemplate());
                buffer->appendChild(templateStr);
                dmx->appendChild(buffer);

                // -- forced
                Poco::AutoPtr<Poco::XML::Element> forcedTag = doc->createElement("Forced");
                bool forced = false;
                for (int j = 0; j < forcedDevices.size(); j++) {
                    if (forcedDevices.at(j) == enabler->getListDmxDevices().at(i)->getName()) {
                        forced = true;
                        break;
                    }
                }
                Poco::AutoPtr<Poco::XML::Text> forcedStr = doc->createTextNode(forced ? "true" : "false");
                forcedTag->appendChild(forcedStr);
                dmx->appendChild(forcedTag);

                // -- type
                Poco::AutoPtr<Poco::XML::Element> typeDmx = doc->createElement("Type");
                std::string typeDmxString = "UNKNOWN";
                if (enabler->getListDmxDevices().at(i)->getType() == DmxDevice::MOVING_HEAD) {
                    typeDmxString = "MOVING_HEAD";
                }
                else if (enabler->getListDmxDevices().at(i)->getType() == DmxDevice::PAR) {
                    typeDmxString = "PAR";

                    PAR* par = static_cast<PAR*>(enabler->getListDmxDevices().at(i));
                    if (par->getCategory() == PAR::TypePAR::FEATURE) {
                        typeDmx->setAttribute("category", "FEATURE");
                    }
                    else if (par->getCategory() == PAR::TypePAR::AMBIANCE) {
                        typeDmx->setAttribute("category", "AMBIANCE");
                    }
                    if (par->getCategory() == PAR::TypePAR::SCENE) {
                        typeDmx->setAttribute("category", "SCENE");
                    }
                }
                else if (enabler->getListDmxDevices().at(i)->getType() == DmxDevice::BACKLIGHT) {
                    typeDmxString = "BACKLIGHT";
                }
                Poco::AutoPtr<Poco::XML::Text> typeDmxStr = doc->createTextNode(typeDmxString);
                typeDmx->appendChild(typeDmxStr);
                dmx->appendChild(typeDmx);

                // -- lut 
                Poco::AutoPtr<Poco::XML::Element> currentLut = doc->createElement("LUT");
                Poco::AutoPtr<Poco::XML::Text> currentLutStr = doc->createTextNode(enabler->getListDmxDevices().at(i)->getCalibName());
                currentLut->appendChild(currentLutStr);
                dmx->appendChild(currentLut);

                KinetConfiguration::DmxDeviceConf* dmxDeviceConf = nullptr;
                for (int j = 0; j < dmxControllerConf->nbLights; j++) {
                    if (dmxControllerConf->lights[j].name == enabler->getListDmxDevices().at(i)->getName()) {
                        dmxDeviceConf = &dmxControllerConf->lights[j];
                        break;
                    }
                }

                // -- luts 
                Poco::AutoPtr<Poco::XML::Element> luts = doc->createElement("LUTS");
                if (dmxDeviceConf != nullptr) {
                    for (int i = 0; i < dmxDeviceConf->nbCalib; i++) {
                        Poco::AutoPtr<Poco::XML::Element> lut = doc->createElement("LUT");
                        Poco::AutoPtr<Poco::XML::Text> lutStr = doc->createTextNode(dmxDeviceConf->calib[i]);
                        lut->appendChild(lutStr);
                        luts->appendChild(lut);
                    }
                }
                dmx->appendChild(luts);
                dmxs->appendChild(dmx);
            }

            device->appendChild(dmxs);
        }

        root->appendChild(device);
    }
}
void ContextResponse::createDownloadedScriptsXml(Poco::XML::Document* doc) 
{
    Poco::AutoPtr<Poco::XML::Element> root = doc->createElement("Downloaded");
    doc->appendChild(root);

    std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>>::iterator itScripts;
    for (itScripts = pathToscripts.begin(); itScripts != pathToscripts.end(); itScripts++) {

        Poco::AutoPtr<Poco::XML::Element> script = doc->createElement("Script");

        // -- name
        Poco::AutoPtr<Poco::XML::Element> name = doc->createElement("Name");
        Poco::AutoPtr<Poco::XML::Text> nameStr = doc->createTextNode(itScripts->first);
        name->appendChild(nameStr);
        script->appendChild(name);

        // -- cis
        Poco::AutoPtr<Poco::XML::Element> cis = doc->createElement("Cis");
        Poco::AutoPtr<Poco::XML::Text> cisStr = doc->createTextNode(itScripts->second->infos.getSha1Cis() + ".cis");
        cis->setAttribute("percent", std::to_string(itScripts->second->percentCis));
        cis->appendChild(cisStr);
        script->appendChild(cis);
        
        // -- lvi
        if (itScripts->second->lviAvailable) {
            Poco::AutoPtr<Poco::XML::Element> lvi = doc->createElement("Lvi");
            Poco::AutoPtr<Poco::XML::Text> lviStr = doc->createTextNode(itScripts->second->infos.getSha1Cis() + ".lvi");
            lvi->appendChild(lviStr);
            script->appendChild(lvi);
        }

        // -- speedRate
        Poco::AutoPtr<Poco::XML::Element> speed = doc->createElement("Speedrate");
        Poco::AutoPtr<Poco::XML::Text> speedStr = doc->createTextNode(std::to_string(itScripts->second->speedRate));
        speed->appendChild(speedStr);
        script->appendChild(speed);

        // -- loop
        Poco::AutoPtr<Poco::XML::Element> loop = doc->createElement("Loop");
        Poco::AutoPtr<Poco::XML::Text> loopStr = doc->createTextNode(itScripts->second->loop ? "true" : "false");
        loop->appendChild(loopStr);
        script->appendChild(loop);

        // -- playable
        Poco::AutoPtr<Poco::XML::Element> playable = doc->createElement("Playable");
        Poco::AutoPtr<Poco::XML::Text> playableStr = doc->createTextNode(itScripts->second->playable ? "true" : "false");
        playable->appendChild(playableStr);
        script->appendChild(playable);

        // -- keep
        Poco::AutoPtr<Poco::XML::Element> keep = doc->createElement("Keep");
        Poco::AutoPtr<Poco::XML::Text> keepStr = doc->createTextNode(itScripts->second->infos.isToKeep() ? "true" : "false");
        keep->appendChild(keepStr);
        script->appendChild(keep);

        // -- played
        Poco::AutoPtr<Poco::XML::Element> played = doc->createElement("Played");
        Poco::AutoPtr<Poco::XML::Text> playedStr = doc->createTextNode(itScripts->second->infos.isAlreadyPlayed() ? "true" : "false");
        played->appendChild(playedStr);
        script->appendChild(played);

        // -- synchros
        Poco::AutoPtr<Poco::XML::Element> syncs = doc->createElement("Syncs");
        for (int i = 0; i < itScripts->second->infos.getNbSync(); i++) {
            Poco::AutoPtr<Poco::XML::Element> sync = doc->createElement("Sync");
            Poco::AutoPtr<Poco::XML::Text> syncStr = doc->createTextNode(itScripts->second->infos.getSyncName(i) + ".sync");
            sync->setAttribute("Present", itScripts->second->syncPresent.at(i) ? "true" : "false");
            sync->appendChild(syncStr);
            syncs->appendChild(sync);
        }
        script->appendChild(syncs);
        root->appendChild(script);
    }
}
void ContextResponse::createModeXml(Poco::XML::Document* doc)
{
    Poco::AutoPtr<Poco::XML::Element> root = doc->createElement("Mode");
    doc->appendChild(root);

    std::string mode = "OFF";
    if (state == ApplicationContext::MANUAL) {
        mode = "MANUAL";
    }
    else if (state == ApplicationContext::SYNC) {
        mode = "SYNC";
    }
    else if (state == ApplicationContext::EXTERNAL) {
        mode = "EXTERNAL";
    }
    Poco::AutoPtr<Poco::XML::Text> modeStr = doc->createTextNode(mode);
    root->appendChild(modeStr);
}
void ContextResponse::createCurrentSyncXml(Poco::XML::Document* doc)
{
    Poco::AutoPtr<Poco::XML::Element> root = doc->createElement("Sync");
    doc->appendChild(root);

    if (currentScript != nullptr) {

        Poco::AutoPtr<Poco::XML::Element> name = doc->createElement("Name");
        Poco::AutoPtr<Poco::XML::Text> nameStr = doc->createTextNode(currentScript->infos.getNameScript());
        name->appendChild(nameStr);
        root->appendChild(name);

        Poco::AutoPtr<Poco::XML::Element> loop = doc->createElement("Loop");
        Poco::AutoPtr<Poco::XML::Text> loopStr = doc->createTextNode(currentScript->loop ? "true" : "false");
        loop->appendChild(loopStr);
        root->appendChild(loop);

        Poco::AutoPtr<Poco::XML::Element> playable = doc->createElement("Playable");
        Poco::AutoPtr<Poco::XML::Text> playableStr = doc->createTextNode(currentScript->playable ? "true" : "false");
        playable->appendChild(playableStr);
        root->appendChild(playable);

        Poco::AutoPtr<Poco::XML::Element> speedRate = doc->createElement("SpeedRate");
        Poco::AutoPtr<Poco::XML::Text> speedRateStr = doc->createTextNode(std::to_string(currentScript->speedRate));
        speedRate->appendChild(speedRateStr);
        root->appendChild(speedRate);

        if (currentSync != nullptr) {
            Poco::AutoPtr<Poco::XML::Element> cplId = doc->createElement("CplId");
            Poco::AutoPtr<Poco::XML::Text> cplIdStr = doc->createTextNode(currentSync->getCplId());
            cplId->appendChild(cplIdStr);
            root->appendChild(cplId);

            Poco::AutoPtr<Poco::XML::Element> cplTitle = doc->createElement("CplTitle");
            Poco::AutoPtr<Poco::XML::Text> cplTitleStr = doc->createTextNode(currentSync->getCplTitle());
            cplTitle->appendChild(cplTitleStr);
            root->appendChild(cplTitle);
        }

        if (scriptDatas != nullptr) {
            Poco::AutoPtr<Poco::XML::Element> frames = doc->createElement("Frames");
            Poco::AutoPtr<Poco::XML::Text> framesStr = doc->createTextNode(std::to_string(scriptDatas->getFrameCount()));
            frames->appendChild(framesStr);
            root->appendChild(frames);
        }
    }
}
void ContextResponse::createCurrentProjectionXml(Poco::XML::Document* doc)
{
    Poco::AutoPtr<Poco::XML::Element> root = doc->createElement("Projection");
    doc->appendChild(root);

    std::string projectionString = "NO_FIT";
    if (projection == PanelProjection::FIT_X) {
        projectionString = "FIT_X";
    }
    else if (projection == PanelProjection::FIT_Y) {
        projectionString = "FIT_Y";
    }
    else if (projection == PanelProjection::FIT_X_PROP_Y) {
        projectionString = "FIT_X_PROP_Y";
    }
    else if (projection == PanelProjection::FIT_Y_PROP_X) {
        projectionString = "FIT_Y_PROP_X";
    }
    else if (projection == PanelProjection::FIT_X_FIT_Y) {
        projectionString = "FIT_X_FIT_Y";
    }
    Poco::AutoPtr<Poco::XML::Text> projStr = doc->createTextNode(projectionString);
    root->appendChild(projStr);
}
void ContextResponse::createConfigurationXml(Poco::XML::Document* doc)
{
    Poco::AutoPtr<Poco::XML::Element> root = doc->createElement("ICE");
    doc->appendChild(root);

    // -- screen
    IceConfiguration::ScreenConf* screenConf = ice->getScreen();
    if (screenConf != nullptr) {
        Poco::AutoPtr<Poco::XML::Element> screen = doc->createElement("screen");
        screen->setAttribute("width", std::to_string(screenConf->width));
        screen->setAttribute("height", std::to_string(screenConf->height));
        screen->setAttribute("offsetX", std::to_string(screenConf->offsetX));
        screen->setAttribute("offsetZ", std::to_string(screenConf->offsetZ));
        root->appendChild(screen);
    }

    // -- seats
    IceConfiguration::SeatConf* seatFLConf = ice->getFrontLeftSeat();
    if (seatFLConf != nullptr) {
        Poco::AutoPtr<Poco::XML::Element> seatFL = doc->createElement("seat");
        seatFL->setAttribute("side", "left");
        seatFL->setAttribute("depth", "front");
        seatFL->setAttribute("offsetX", std::to_string(seatFLConf->offsetX));
        seatFL->setAttribute("offsetY", std::to_string(seatFLConf->offsetY));
        seatFL->setAttribute("offsetZ", std::to_string(seatFLConf->offsetZ));
        root->appendChild(seatFL);
    }
    IceConfiguration::SeatConf* seatRRConf = ice->getRearRightSeat();
    if (seatRRConf != nullptr) {
        Poco::AutoPtr<Poco::XML::Element> seatRR = doc->createElement("seat");
        seatRR->setAttribute("side", "right");
        seatRR->setAttribute("depth", "reae");
        seatRR->setAttribute("offsetX", std::to_string(seatRRConf->offsetX));
        seatRR->setAttribute("offsetY", std::to_string(seatRRConf->offsetY));
        seatRR->setAttribute("offsetZ", std::to_string(seatRRConf->offsetZ));
        root->appendChild(seatRR);
    }

    // -- panels
    for (int i = 0; i < ice->getNbPanels(); i++) {
        Poco::AutoPtr<Poco::XML::Element> panel = doc->createElement("panel");
        panel->setAttribute("name", ice->getPanel(i)->name);
        panel->setAttribute("side", ice->getPanel(i)->side == IceConfiguration::LEFT ? "left" : "right");
        panel->setAttribute("offsetX", std::to_string(ice->getPanel(i)->offsetX));
        panel->setAttribute("offsetY", std::to_string(ice->getPanel(i)->offsetY));
        panel->setAttribute("offsetZ", std::to_string(ice->getPanel(i)->offsetZ));
        panel->setAttribute("width", std::to_string(ice->getPanel(i)->width));
        panel->setAttribute("height", std::to_string(ice->getPanel(i)->height));
        root->appendChild(panel);
    }

    // -- moving heads
    for (int i = 0; i < ice->getNbMovingHeads(); i++) {
        Poco::AutoPtr<Poco::XML::Element> mh = doc->createElement("movingHead");
        mh->setAttribute("name", ice->getMovingHead(i)->name);
        mh->setAttribute("side", ice->getMovingHead(i)->left ? "left" : "right");
        mh->setAttribute("depth", ice->getMovingHead(i)->front ? "front" : "rear");
        mh->setAttribute("offsetX", std::to_string(ice->getMovingHead(i)->offsetX));
        mh->setAttribute("offsetY", std::to_string(ice->getMovingHead(i)->offsetY));
        mh->setAttribute("offsetZ", std::to_string(ice->getMovingHead(i)->offsetZ));
        mh->setAttribute("panFullAngle", std::to_string(ice->getMovingHead(i)->panFullAngle));
        mh->setAttribute("tiltFullAngle", std::to_string(ice->getMovingHead(i)->tiltFullAngle));
        root->appendChild(mh);
    }
}