#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>

#pragma once

#include "Poco/File.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "syncFile.h"
#include "Poco/Logger.h"
#include "synchronizable.h"

class LVI : public Synchronizable
{

public:

    LVI(std::string pathFile);
    ~LVI();

    bool load();
    bool save();

    void setVersion(std::string version) {this->version = version;}
    void setName(std::string name) {this->name = name;}
    void setType(std::string type) {this->type = type;}
    void setSha1(std::string sha1) {this->sha1 = sha1;}
    void setLoop(bool loop) {this->loop = loop;}
    void setDuration(int duration) {this->duration = duration;}
    bool isLoop() {return loop;}


    void setStartValues(int startDelay, int entryPoint) {this->startDelay = startDelay;this->entryPoint = entryPoint;}
    int getStartDelay() {return startDelay;}
    int getEntryPoint() {return entryPoint;}

private:

    std::string version;
    std::string name;
    std::string type; // --
    std::string sha1;
    bool loop;
    std::string referenceFrame; // --
    std::string fps; // --
    std::string editRate; // --
    int duration; // -- 
    std::string comments; // --
};