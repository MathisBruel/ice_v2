#include <cstdlib>
#include <iostream>
#include <map>

#pragma once
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/Logger.h"
#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"
#include "image.h"
#include "complexParam.h"

class Command
{

public:

    // -- list of all commands types
    enum CommandType {

        // -- CONTEXT
        STATUS_IMS,
        
        // -- DEVICES
        GET_CONFIGURATION,
        LIST_DEVICES,
        UNFORCE_DEVICE,
        UNFORCE_ALL_DEVICES,
        SWITCH_LUT,

        GET_MODE,
        SWITCH_MODE,

        LAUNCH_SCRIPT,
        STOP_SCRIPT,

        GET_CURRENT_PROJECTION,
        SWITCH_PROJECTION,

        // -- DRAW DEVICES
        SET_COLOR,
        SET_IMAGE,
        SET_POINTER,

        // -- SPECIFIC FOR HALLWAY
        SET_PAR_SCENE_ON,
        SET_PAR_SCENE_OFF,
        SET_PAR_AMBIANCE_ON,
        SET_PAR_AMBIANCE_OFF,

        // -- PLAYER
        PLAY_TEMPLATE_IMAGE,

        // -- IMS
        GET_CPL_INFOS,
        GET_CPL_CONTENT,
        GET_CPL_CONTENT_NAME,

        // -- REPOSITORY
        GET_DOWNLOADED_SCRIPTS,
        GET_CURRENT_SYNC,
        CHANGE_PRIORITY_SCRIPT,
        SYNCHRONIZE_SCRIPT,
        GET_SYNC_CONTENT,

        // -- SCENES
        GET_SCENES,
        PLAY_SCENE,
        CREATE_UPDATE_SCENE,
        REMOVE_SCENE,

        // -- DEFAULT
        UNKNOW_COMMAND
    };

    // -- define which thread must treat the command
    enum CommandDispatch {
        KINET,
        IMS,
        REPOSITORY,
        PLAYER,
        CONTEXT,
        UNKNOW
    };

    Command();
    Command(CommandType type);
    ~Command();

    // -- parse XML command
    bool loadFromXmlContent(std::istream& content);
    // -- set datas to understand command
    void addParameter(std::string key, std::string value);
    void setImage(std::shared_ptr<Image> img) {this->image = img;}

    // -- get datas of command to different types
    int getIntParameter(std::string key, int defaultValue = -1);
    std::string getStringParameter(std::string key, std::string defaultValue = "");
    double getDoubleParameter(std::string key, double defaultValue = -1.0);
    bool getBoolParameter(std::string key, bool defaultValue = false);
    std::shared_ptr<Image> getImageParameter() {return image;}
    ComplexParam* getComplexParam(std::string name);

    std::map<std::string, std::string> getParameters() {return parameters;}

    CommandType getType() {return type;}
    CommandDispatch getDispatch() {return dispatch;}
    std::string getUuid() {return uuid;}

    // -- enum transformation from/to string
    static CommandType generateFromString(std::string typeStr);
    static std::string generateToString(CommandType type);

    // -- dispatch command and validate it is well-formatted
    bool validateCommand();
    void setDispatch();

    void setTimingsResponse(int nbRetry, int waitBetweenRetry) {this->nbRetry = nbRetry; this->waitBetweenRetry = waitBetweenRetry;}
    int getNbRetry() {return nbRetry;}
    int getWaitBetweenRetry() {return waitBetweenRetry;}

    void setTimestamp(uint64_t timestamp) {this->timestamp = timestamp;}
    uint64_t getTimestamp() {return timestamp;}

private:

    CommandType type;
    CommandDispatch dispatch;

    std::map<std::string, std::string> parameters;
    std::map<std::string, ComplexParam> complex;

    std::shared_ptr<Image> image;

    // -- timestamp of reception to check if timeout
    uint64_t timestamp;

    // -- same as in the response
    std::string uuid;

    // -- timings for waiting response
    int nbRetry;
    int waitBetweenRetry; // -- µs
};