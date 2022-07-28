#include <cstdlib>
#include <iostream>

#pragma once

#include "KinetConfiguration.h"
#include "IMSConfiguration.h"
#include "IceConfiguration.h"
#include "LutConfiguration.h"
#include "RepoConfiguration.h"
#include "SceneConfiguration.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/Logger.h"

class Configuration
{

public:

    // -- entry point of all configuration files

    Configuration(std::string path);
    ~Configuration();

    bool loadConfiguration();
    std::string toString();

    KinetConfiguration* getKinetConf() {return kinetConf;}
    IMSConfiguration* getImsConf() {return imsConf;}
    IceConfiguration* getIceConf() {return iceConf;}
    LutConfiguration* getLutConf() {return lutConf;}
    RepoConfiguration* getRepoConf() {return repoConf;}
    SceneConfiguration* getSceneConf() {return sceneConf;}

private:

    // -- path of configuration files
    std::string path;

    // -- list of equipment with name, serials, ip and dmx addressing and formatting
    KinetConfiguration* kinetConf;

    // -- definition of IMS : how to connect, type and protocol
    IMSConfiguration* imsConf;

    // -- list of XYZ placement of all devices in the room
    IceConfiguration* iceConf;

    // -- definition of path for color of corrections
    LutConfiguration* lutConf;

    // -- data to synchronize with central database : connection, user
    RepoConfiguration* repoConf;

    // -- definition of scenes
    SceneConfiguration* sceneConf;
};