#include <cstdlib>
#include <iostream>
#include <map>


#pragma once
#include "Configuration.h"

// -- IMS
#include "ImsCommunicator.h"
#include "device/DoremiHandler.h"
#include "device/BarcoHandler.h"
#include "device/ChristieHandler.h"

// -- KINET
#include "KinetMulticast.h"
#include "device/Device.h"
#include "device/LuminousTextile.h"
#include "device/DataEnabler.h"
#include "device/DmxDevice.h"
#include "device/Backlight.h"
#include "device/Par.h"
#include "device/MovingHead.h"

// -- RENDERER
#include "Template.h"
#include "TemplateV1.h"
#include "MovingHeadProjection.h"
#include "PanelProjection.h"

// -- REPO
#include "CentralRepository.h"
#include "LocalRepository.h"
#include "FtpConnector.h"
#include "cis.h"
#include "syncFile.h"

// -- COMMANDS
#include "Poco/Net/HTTPServer.h"
#include "httpCommandReceiver.h"
#include "udpCommandReceiver.h"
#include "streamReceiver.h"

// -- MUTEXS
#include "Poco/Mutex.h"
#include "FrameOutputs.h"
#include "portable_timer.h"

class ApplicationContext
{

public:

    enum PlayerState {
        OFF,
        MANUAL,
        EXTERNAL,
        SYNC
    };

    static ApplicationContext* getCurrentContext();

    bool initConfiguration(std::string pathConfigFiles);
    bool initIms();
    bool initKinet();
    bool initRenderer();
    bool initRepos();
    bool initHttpCommand();
    bool initUdpCommand();
    bool initStreamReceiver();
    void stopComponents();

    Configuration* getConfig() {return config;}
    
    // -- Ims
    ImsCommunicator* getIms() {return ims;}
    std::shared_ptr<PlayerStatus> getPlayerStatus() {return statusPlayer;}
    void setPlayerStatus(std::shared_ptr<PlayerStatus> status);

    // -- CMD
    HttpCommandHandler* getCommandHandler() {return commandHandler;}
    UdpCommandReceiver* getCommandUdpReceiver() {return commandUdpReceiver;}
    StreamReceiver* getStreamReceiver() {return streamer;}

    // -- KINET
    KinetMulticast* getKinet() {return kinet;}
    std::map<std::string, Device*> getListDevices() {return listDevices;}
    void forceDevice(std::string name);
    void unforceDevice(std::string name);
    void unforceAllDevices();
    bool isDeviceForced(std::string name);
    std::vector<std::string> getForcedDevices() {return forcedDevices;}

    // -- RENDERER
    MovingHeadProjection* getMovingHeadProjector() {return movingHead;}
    PanelProjection::ProjectionType getProjectionType();
    PanelProjection::ProjectionType getBaseProjectionType() {return projectionType;}
    void setBaseProjectionType(PanelProjection::ProjectionType projectionType) {this->projectionType = projectionType;}
    void setCurrentExternalFrame(int currentExternalFrame) {
        while (!tryLockScriptsMutex()) {Timer::crossUsleep(2);}
        this->currentExternalFrame = currentExternalFrame;
        unlockScriptsMutex();
    }

    // -- PLAYER
    void handleSynchronization();
    void setPlayerState(PlayerState state) {this->state = state;}

    void setCurrentScript(std::shared_ptr<LocalRepository::ScriptsInformations> currentScript);
    void setCurrentSync(std::shared_ptr<SyncFile> currentSync);
    void setScriptDatas(std::shared_ptr<Cis> scriptDatas);

    void setTimestampStartLoop(int64_t timestampStartLoop) {this->timestampStartLoop = timestampStartLoop;}
    void setForceScript() {scriptForce = true;}
    void unforceScript() {scriptForce = false;}
    bool isScriptForced() {return scriptForce;}
    PlayerState getPlayerState() {return state;}
    std::shared_ptr<LocalRepository::ScriptsInformations> getCurrentScript() {return currentScript;}
    std::shared_ptr<SyncFile> getCurrentSync() {return currentSync;}
    std::shared_ptr<Cis> getScriptDatas() {return scriptDatas;}
    int64_t getTimestampStartLoop() {return timestampStartLoop;}
    void stopCurrentScript();
    void playAllBlack(bool forced = false);

    // -- CONTEXT
    int getCurrentFrame();

    // -- REPO
    LocalRepository* getRepoLocal() {return repoLocal;}
    CentralRepository* getRepoCentral() {return repoCentral;}

    // -- MUTEXS
    bool tryLockStatusPlayerMutex() {return statusPlayerMutex.tryLock();}
    bool tryLockListDevicesMutex() {return listDevicesMutex.tryLock();}
    bool tryLockKinetSenderMutex() {return kinetSenderMutex.tryLock();}
    bool tryLockProjectionMutex() {return projectionMutex.tryLock();}
    bool tryLockOutputsMutex() {return outputsMutex.tryLock();}
    bool tryLockForcedDevicesMutex() {return forcedDevicesMutex.tryLock();}
    bool tryLockLocalRepoMutex() {return localRepoMutex.tryLock();}
    bool tryLockCentralRepoMutex() {return centralRepoMutex.tryLock();}
    bool tryLockModeMutex() {return modeMutex.tryLock();}
    bool tryLockScriptsMutex() {return scriptsMutex.tryLock();}
    bool tryLockCplsMutex() {return cplsMutex.tryLock();}

    void unlockStatusPlayerMutex() {statusPlayerMutex.unlock();}
    void unlockListDevicesMutex() {listDevicesMutex.unlock();}
    void unlockKinetSenderMutex() {kinetSenderMutex.unlock();}
    void unlockProjectionMutex() {projectionMutex.unlock();}
    void unlockOutputsMutex() {outputsMutex.unlock();}
    void unlockForcedDevicesMutex() {forcedDevicesMutex.unlock();}
    void unlockLocalRepoMutex() {localRepoMutex.unlock();}
    void unlockCentralRepoMutex() {centralRepoMutex.unlock();}
    void unlockModeMutex() {modeMutex.unlock();}
    void unlockScriptsMutex() {scriptsMutex.unlock();}
    void unlockCplsMutex() {cplsMutex.unlock();}

    void insertPanelsOutput(int frameToCompute, std::shared_ptr<OutputPanelImage> output);
    void insertMovingHeadOutput(int frameToCompute, std::shared_ptr<MovingHeadProjection::OutputMovingHeadAngle> output);
    void insertParOutput(int frameToCompute, std::shared_ptr<Template::ParOutputs> output);
    void insertBacklightOutput(int frameToCompute, std::shared_ptr<Template::BacklightOutputs> output);

    void addNewOutput(int frameToCompute, std::shared_ptr<FrameOutputs> output);
    bool hasFrameOutput(int frameToCompute);
    std::shared_ptr<FrameOutputs> getOutputs(int currentFrame);
    void removeOldOutput();

    void addCPLInfo(std::shared_ptr<CplInfos> cplInfo);
    void removeCPLInfo(std::string cplId);
    bool hasCplId(std::string cplId);
    std::shared_ptr<CplInfos> getCplsInfo(std::string cplId);
    std::map<std::string, std::shared_ptr<CplInfos>> getCplsInfos() {return cpls;}

private:

    ApplicationContext();
    ~ApplicationContext();

    static ApplicationContext* context;

    // -- configurations
    Configuration* config;

    // -- ims sender/listener
    ImsCommunicator* ims;
    std::shared_ptr<PlayerStatus> statusPlayer;
    Poco::Mutex statusPlayerMutex;
    std::map<std::string, std::shared_ptr<CplInfos>> cpls;
    Poco::Mutex cplsMutex;

    // -- kinet sender/listener
    KinetMulticast* kinet;
    std::map<std::string, Device*> listDevices;
    Poco::Mutex listDevicesMutex;
    Poco::Mutex kinetSenderMutex;

    // -- Version 1
    // workers
    MovingHeadProjection* movingHead;
    PanelProjection::ProjectionType projectionType;
    Poco::Mutex projectionMutex;
    // -- others template

    // datas : outputs by frame
    std::map<int, std::shared_ptr<FrameOutputs>> outputs;
    Poco::Mutex outputsMutex;
    std::vector<std::string> forcedDevices;
    Poco::Mutex forcedDevicesMutex;
    int currentExternalFrame;

    // -- REPOSITORY
    CentralRepository* repoCentral;
    LocalRepository* repoLocal;
    Poco::Mutex localRepoMutex;
    Poco::Mutex centralRepoMutex;

    // -- general to identify what is playing
    PlayerState state;
    Poco::Mutex modeMutex;

    std::shared_ptr<LocalRepository::ScriptsInformations> currentScript;
    std::shared_ptr<SyncFile> currentSync;
    std::shared_ptr<Cis> scriptDatas;
    int64_t timestampStartLoop;
    bool scriptForce;
    Poco::Mutex scriptsMutex;

    // -- http command server
    HttpCommandHandler* commandHandler;
    Poco::Net::HTTPServer* server;
    StreamReceiver* streamer;

    // -- udp command receiver (for IMS)
    UdpCommandReceiver* commandUdpReceiver;
};