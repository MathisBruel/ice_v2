#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

#pragma once

// -- DATABASE
#include "DatabaseConnector.h"
#include "Query.h"
#include "ResultQuery.h"
#include "StateServer.h"
#include "DownloadedScripts.h"
#include "PresentCpls.h"
#include "ScriptState.h"

// -- DATA MODEL
#include "Data/Group.h"
#include "Data/Cinema.h"
#include "Data/Script.h"
#include "Data/Feature.h"
#include "Data/LinkParam.h"

// -- IMS
#include "ImsCommunicator.h"
#include "device/DoremiHandler.h"
#include "device/BarcoHandler.h"
#include "device/ChristieHandler.h"

// -- COMMANDS
#include "Poco/Net/HTTPServer.h"
#include "httpCentralReceiver.h"

// -- MUTEXS
#include "Poco/Mutex.h"
#include "portable_timer.h"

class CentralContext
{

public:

    static CentralContext* getCurrentContext();
    bool startHttpCmdHandler();
    bool initDatabase();

    HttpCentralHandler* getCommandHandler() {return commandHandler;}
    ResultQuery* executeQuery(Query* query);

    bool tryLockDatabaseMutex() {return mutexDatabase.tryLock();}
    void unlockDatabaseMutex() {mutexDatabase.unlock();}
    bool tryLockDataMutex() {return mutexData.tryLock();}
    void unlockDataMutex() {mutexData.unlock();}
    bool tryLockSessionICEMutex() {return mutexICE.tryLock();}
    void unlockSessionICEMutex() {mutexICE.unlock();}

    // -- at execution to access protected datas
    void addStateAuditorium(std::shared_ptr<StateServer> state);
    void addStateServer(std::shared_ptr<StateServer> state);
    std::map<int, std::shared_ptr<StateServer>> getStateAuditoriums() {return stateAuditoriums;}
    std::map<int, std::shared_ptr<StateServer>> getStateServers() {return stateServers;}
    void removeStateAuditorium(int id_auditorium);
    void removeStateServer(int id_server);

    void addDownloadedScripts(std::shared_ptr<DownloadedScripts> downloaded);
    void addPresentCpls(std::shared_ptr<PresentCpl> presents);
    std::map<int, std::shared_ptr<DownloadedScripts>> getDownloadedScripts() {return scripts;}
    std::map<int, std::shared_ptr<PresentCpl>> getPresentCpls() {return cpls;}
    void removeDownloadedScripts(int id_server);
    void removePresentCpls(int id_server);

    void addScriptsInfos(int id_release, std::shared_ptr<ScriptState> info);
    std::map<int, std::shared_ptr<ScriptState>> getScriptsInfos() {return scriptServerForRelease;}
    void removeScriptsInfos(int id_release);

    void addOutputSynchro(std::string uuidCpl, std::string output);
    std::map<std::string, std::string> getOutputsSynchro() {return outputsSynchro;}
    void removeOutputSynchro(std::string uuidCpl);

    // -- preload database for working
    std::map<int, std::shared_ptr<Auditorium>> prepareAuditoriums();
    std::map<int, std::shared_ptr<Server>> prepareServers();
    std::map<int, std::shared_ptr<Release>> prepareRelease();
    std::map<int, std::shared_ptr<Cinema>> prepareCinemas();

    std::vector<int> associateRelatedCinema(std::shared_ptr<Release> release);

    DatabaseConnector* getDatabaseConnector() {return database;}

private:

    CentralContext();
    ~CentralContext();

    static CentralContext* context;

    // -- database central
    DatabaseConnector* database;
    Poco::Mutex mutexDatabase;
    Poco::Mutex mutexData;
    Poco::Mutex mutexICE;
    HttpCentralHandler* commandHandler;

    // ---------------
    // FOR OUTPUT
    // ---------------
    // -- state of servers and auditoriums
    Poco::Mutex mutexState;
    std::map<int, std::shared_ptr<StateServer>> stateAuditoriums;
    std::map<int, std::shared_ptr<StateServer>> stateServers;

    Poco::Mutex mutexReleaseData;
    // -- state of script for a release foreach auditorium
    std::map<int, std::shared_ptr<ScriptState>> scriptServerForRelease;
    // -- used to get result of synchronization
    std::map<std::string, std::string> outputsSynchro;

    // ---------------
    // CROSS THREADS
    // ---------------
    Poco::Mutex mutexServerData;
    // -- scripts present on ice auditorium
    std::map<int, std::shared_ptr<DownloadedScripts>> scripts;
    // -- cpl present on auditorium
    std::map<int, std::shared_ptr<PresentCpl>> cpls;
};