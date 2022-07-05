#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>

#pragma once
#include "Poco/File.h"
#include "Poco/Path.h"
#include "Poco/Logger.h"
#include "Poco/Timestamp.h"
#include "Poco/JSON/Parser.h"
#include "ScriptInfoFile.h"
#include "Converter.h"

class LocalRepository
{

public:

    struct ScriptsInformations {

        uint64_t timestamp;
        ScriptInfoFile infos;
        
        bool loop;
        bool playable;
        double speedRate;

        bool cisAvailable;
        double percentCis;
        bool lviAvailable;
        std::vector<bool> syncPresent;
    };

    LocalRepository(std::string pathBase);
    ~LocalRepository();

    bool setPriorityScript(bool keep, std::string scriptName);
    void updateScriptsInformations();
    void checkMemoryUsage();
    void allocateSpace();

    bool isSpaceWarning() {return warning;}

    std::shared_ptr<ScriptsInformations> getScriptFromCplId(std::string cplId);
    std::map<std::string, std::shared_ptr<ScriptsInformations>> getPathToScripts() {return pathToScripts;}
    std::map<std::string, std::shared_ptr<ScriptsInformations>> getCplIdToscripts() {return cplIdToScripts;}

    std::string getBasePath() {return pathBase;}

private:

    // -- base of all scripts
    std::string pathBase;

    // -- list of all scripts
    std::map<std::string, std::shared_ptr<ScriptsInformations>> pathToScripts;
    std::map<std::string, std::shared_ptr<ScriptsInformations>> cplIdToScripts;

    // -- memory management 
    long diskSpaceTotal;
    long diskSpaceFree;
    bool warning;
};