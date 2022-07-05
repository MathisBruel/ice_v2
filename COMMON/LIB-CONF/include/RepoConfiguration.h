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

class RepoConfiguration
{

public:

    enum ConnectorType {
        FTP,
        HTTP
    };
    enum TypeServer {
        ROOM,
        DOORWAY
    };
    enum RightsServer {
        QC,
        CLASSIC
    };

    RepoConfiguration(std::string filename);
    ~RepoConfiguration();

    bool load();

    std::string getHost() {return host;}
    uint16_t getPort() {return port;}
    std::string getUsername() {return username;}
    std::string getPassword() {return password;}
    TypeServer getTypeServer() {return type;}
    RightsServer getRights() {return rights;}
    std::string getPathScripts() {return pathScripts;}

    std::string getHostDownload() {return hostDownload;}
    uint16_t getPortDownload() {return portDownload;}
    std::string getUsernameDownload() {return usernameDownload;}
    std::string getPasswordDownload() {return passwordDownload;}
    std::string getCinema() {return cinema;}
    ConnectorType getConnectorType() {return connector;}

    std::string toString();

private:

    // -- to connect to FTP or HTTP
    std::string host;
    uint16_t port;
    std::string username;
    std::string password;

    // -- to connect to HTTP download
    std::string hostDownload;
    uint16_t portDownload;
    std::string usernameDownload;
    std::string passwordDownload;
    std::string cinema;

    // -- where to download scripts
    std::string pathScripts;

    // -- define type server
    TypeServer type;
    RightsServer rights;
    ConnectorType connector;

    // -- reference path to parse configurtation file
    std::string pathFile;
};
