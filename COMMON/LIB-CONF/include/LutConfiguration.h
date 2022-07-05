#include <cstdlib>
#include <iostream>
#include <fstream>

#pragma once

#include "Poco/File.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "lut.h"
#include "calibrer.h"
#include "Poco/Logger.h"

class LutConfiguration
{

public:

    LutConfiguration(std::string filename);
    ~LutConfiguration();

    bool load();

    LUT* getLutWithName(std::string name);
    Calibrer* getCalibWithName(std::string name);

    std::string toString();

private:

    std::string pathFile;

    std::map<std::string, LUT*> luts;
    std::map<std::string, Calibrer*> calibs;
};