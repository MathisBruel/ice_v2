#include <cstdlib>
#include <iostream>
#include <set>

#include "cutscenes.h"
#include "Poco/JSON/Parser.h"
#include "Poco/DynamicStruct.h"
#include <fstream>

#pragma once
#include "PanelProjection.h"
#include "command.h"
#include "synchronizable.h"

class SyncFile : public Synchronizable
{

public:

    SyncFile(std::string pathFile);
    ~SyncFile();
    
    bool load();
    bool save();

    // -- CUSTOM PARAMETERS
    void setVersion(std::string version) {this->version = version;}
    void setType(std::string type) {this->type = type;}
    void setSha1(std::string sha1) {this->sha1 = sha1;}

    // -- TIMINGS VALUE
    void setStartValues(int startDelay, int entryPoint) {this->startDelay = startDelay;this->entryPoint = entryPoint;}
    int getStartDelay() {return startDelay;}
    int getEntryPoint() {return entryPoint;}

    // -- SPECIFIC FOR PLAY SCRIPT
    int getFrameFromCplDurationMs(double currentMs);

private:

    std::string version;
    std::string type;
    std::string sha1;
};