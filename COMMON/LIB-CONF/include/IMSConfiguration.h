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
#include "Converter.h"
#include "Poco/Logger.h"

class IMSConfiguration
{

public:

    IMSConfiguration(std::string filename);
    ~IMSConfiguration();

    bool load();

    std::string getIpIms() {return ip;}
    uint16_t getPort() {return port;}
    uint8_t getSyncRate() {return syncRate;}
    std::string getTypeIms() {return type;}
    std::string getUsername() {return username;}
    std::string getPassword() {return password;}
    std::string getInterface() {return interfaceName;}

    int getOffsetMs2K() {return offsetMs2K;}
    int getOffsetMs4K() {return offsetMs4K;}

    std::string toString();

private:

    std::string pathFile;

    std::string ip;     // -- to connect with IMS
    uint16_t port;      // -- port vary with type of IMS
    uint8_t syncRate;   // -- in milliseconds

    // -- deduct protocol HTTP or UDP
    std::string type;
    std::string interfaceName;

    // -- already decoded
    std::string username;
    std::string password;

    int offsetMs2K;
    int offsetMs4K;
};
