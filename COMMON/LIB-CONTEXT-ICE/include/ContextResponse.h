#include <cstdlib>
#include <iostream>

#pragma once
#include "commandResponse.h"
#include "ApplicationContext.h"

class ContextResponse : public CommandResponse
{

public:

    // -- builder of XML responses to HTTP commands

    ContextResponse(std::string uuidFromCommand, Command::CommandType type);
    ~ContextResponse();

    void setStatusPlayerIms(std::shared_ptr<PlayerStatus> statusPlayer) {this->statusPlayer = statusPlayer;}
    void setCplInfos(std::map<std::string, std::shared_ptr<CplInfos>> infos) {this->listOfCpls = infos;}
    void setListDevices(std::map<std::string, Device*> listDevices) {this->listDevices = listDevices;}

    void setDownloadedScripts(std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>> pathToscripts) {this->pathToscripts = pathToscripts;}
    void setForcedDevices(std::vector<std::string> forcedDevices) {this->forcedDevices = forcedDevices;}
    void setMode(ApplicationContext::PlayerState state) {this->state = state;}
    void setCurrentSync(std::shared_ptr<LocalRepository::ScriptsInformations> currentScript, std::shared_ptr<SyncFile> currentSync, std::shared_ptr<Cis> scriptDatas, uint64_t timestampStartLoop);
    void setProjection(PanelProjection::ProjectionType projection) {this->projection = projection;}
    void setAvailableScripts(std::vector<std::string> availableScripts) {this->availableScripts = availableScripts;}

    void setIceConfiguration(IceConfiguration* ice) {this->ice = ice;}
    void setKinetConfiguration(KinetConfiguration* kinet) {this->kinet = kinet;}
    void setSceneConf(SceneConfiguration* sceneConf) {this->sceneConf = sceneConf;}

    void fillDatas();

private:

    void createPlayerStatusXml(Poco::XML::Document* doc);
    void createCplListXml(Poco::XML::Document* doc);
    void createListDevicesXml(Poco::XML::Document* doc);
    void createDownloadedScriptsXml(Poco::XML::Document* doc);
    void createModeXml(Poco::XML::Document* doc);
    void createCurrentSyncXml(Poco::XML::Document* doc);
    void createCurrentProjectionXml(Poco::XML::Document* doc);
    void createConfigurationXml(Poco::XML::Document* doc);
    void createAvailableScriptsXml(Poco::XML::Document* doc);

    // -- GET STATUS IMS
    std::shared_ptr<PlayerStatus> statusPlayer;
    std::map<std::string, std::shared_ptr<CplInfos>> listOfCpls;

    // -- GET LIST DEVICES
    // -- GET CURRENT LUT
    std::map<std::string, Device*> listDevices;
    KinetConfiguration* kinet;
    // -- GET_CONFIGURATION
    IceConfiguration* ice;

    // -- GET DOWNLOADED SCRIPTS
    std::map<std::string, std::shared_ptr<LocalRepository::ScriptsInformations>> pathToscripts;

    // -- GET FORCED DEVICES
    std::vector<std::string> forcedDevices;

    // -- GET MODE
    ApplicationContext::PlayerState state;

    // -- GET CURRENT SYNC
    std::shared_ptr<LocalRepository::ScriptsInformations> currentScript;
    std::shared_ptr<SyncFile> currentSync;
    std::shared_ptr<Cis> scriptDatas;
    uint64_t timestampStartLoop;

    // -- GET_PROJECTION
    PanelProjection::ProjectionType projection;

    // -- GET SCENES
    SceneConfiguration* sceneConf;

    // -- AVAILABLE SCRIPTS
    std::vector<std::string> availableScripts;
};