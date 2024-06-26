#include <cstdlib>
#include <iostream>
#include <map>
#include <fstream>

#pragma once
#include "Poco/DOM/DOMParser.h"
#include "Poco/XML/XMLWriter.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/Logger.h"
#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"

class CommandCentral
{

public:

    enum CommandCentralType {

        // ------------------------ 
        // DATABASE HANDLE
        // ------------------------ 
        // -- GROUPS
        GET_GROUPS,
        INSERT_GROUP,
        UPDATE_GROUP,
        DELETE_GROUP,
        // -- CINEMA
        GET_CINEMAS,
        INSERT_CINEMA,
        UPDATE_CINEMA,
        DELETE_CINEMA,
        // -- AUDITORIUM
        GET_AUDITORIUMS,
        INSERT_AUDITORIUM,
        UPDATE_AUDITORIUM,
        DELETE_AUDITORIUM,
        // -- SERVER
        GET_SERVERS,
        INSERT_SERVER,
        UPDATE_SERVER,
        DELETE_SERVER,
        // -- SCRIPTS
        GET_SCRIPTS,
        INSERT_SCRIPT,
        UPDATE_SCRIPT,
        DELETE_SCRIPT,
        // -- FEATURES
        GET_FEATURES,
        INSERT_FEATURE,
        UPDATE_FEATURE,
        DELETE_FEATURE,
        // -- RELEASES
        GET_RELEASES,
        INSERT_RELEASE,
        UPDATE_RELEASE,
        DELETE_RELEASE,
        // -- CPLS
        GET_CPLS,
        INSERT_CPL,
        UPDATE_CPL,
        DELETE_CPL,
        // -- CUTS
        GET_CUT,
        INSERT_CUT,
        UPDATE_CUT,
        DELETE_CUT,

        // -- ASSOCIATIONS GROUP/CINEMA
        ADD_CINEMA_TO_GROUP,
        REMOVE_CINEMA_TO_GROUP,
        // -- ASSOCIATIONS RELEASE
        ADD_CPL_TO_RELEASE,
        REMOVE_CPL_TO_RELEASE,
        ADD_CPL_TO_SCRIPT,
        REMOVE_CPL_TO_SCRIPT,
        ADD_SCRIPT_TO_RELEASE,
        REMOVE_SCRIPT_TO_RELEASE,
        ADD_CINEMA_TO_RELEASE,
        REMOVE_CINEMA_TO_RELEASE,
        ADD_GROUP_TO_RELEASE,
        REMOVE_GROUP_TO_RELEASE,

        // ------------------------ 
        // INFOS FOR WEB INTERFACE
        // ------------------------ 
        GET_STATE_SERVER,
        GET_STATE_AUDITORIUM,
        GET_SCRIPTS_FOR_RELEASE,
        GET_RESULT_SYNCHRO,

        // ------------------------ 
        // -- ICE destination
        // ------------------------ 
        GET_SCRIPTS_FOR_SERVER,
        // -- specific REPO service
        GET_FILE,

        // ------------------------
        // -- State Machine Interaction
        // ------------------------
        CREATE_CONTENT,
        CONTENT_CREATED,
        RELEASE_CREATED,
        CREATE_RELEASE,
        CIS_CREATED,
        CREATE_CPL,
        CREATE_SYNCLOOP,
        CPL_CREATED,
        SYNC_CREATED,
        SYNCLOOP_CREATED,
        IMPORT_TO_PROD,
        
        // -- DEFAULT
        UNKNOW_COMMAND
    };

    CommandCentral();
    CommandCentral(CommandCentralType type);
    ~CommandCentral();

    bool loadFromXmlContent(std::istream& content);
    void addParameter(std::string key, std::string value);

    int getIntParameter(std::string key, int defaultValue = -1);
    std::string getStringParameter(std::string key, std::string defaultValue = "");
    double getDoubleParameter(std::string key, double defaultValue = -1.0);
    bool getBoolParameter(std::string key, bool defaultValue = false);

    std::map<std::string, std::string> getParameters() {return parameters;}

    CommandCentralType getType() {return type;}
    std::string getUuid() {return uuid;}

    static CommandCentralType generateFromString(std::string typeStr);
    static std::string generateToString(CommandCentralType type);

    bool validateCommand();

    void setTimingsResponse(int nbRetry, int waitBetweenRetry) {this->nbRetry = nbRetry; this->waitBetweenRetry = waitBetweenRetry;}
    int getNbRetry() {return nbRetry;}
    int getWaitBetweenRetry() {return waitBetweenRetry;}

    void setTimestamp(uint64_t timestamp) {this->timestamp = timestamp;}
    uint64_t getTimestamp() {return timestamp;}

private:

    CommandCentralType type;
    std::map<std::string, std::string> parameters;

    // -- timestamp of reception to check if timeout
    uint64_t timestamp;

    // -- same as in the response
    std::string uuid;

    // -- timings for waiting response
    int nbRetry;
    int waitBetweenRetry; // -- µs
};