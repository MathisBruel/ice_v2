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
#include "Poco/Logger.h"

class KinetConfiguration
{

public:

    enum DmxType {
        BACKLIGHT,
        PAR,
        MOVINGHEAD,
        UNKNOW
    };

    struct PanelConf {
        // -- to be find versus template
        std::string name;
        std::string ip;
        uint32_t serial;

        // -- define size
        uint8_t sizePixel;
        uint16_t nbPixels;

        // -- lut
        int nbLut;
        std::string* lutPriority;
    };

    struct DmxDeviceConf {
        // -- to be find versus template
        std::string name;
        DmxType type;
        std::string category;

        // -- to format payload
        uint16_t dmxOffset;
        std::string formatBuffer;

        uint8_t defaultZoom;
        uint16_t lumens;

        // -- calib
        int nbCalib;
        std::string* calib;
        std::string refCalib;
    };

    struct DmxControlerConf {
        std::string name;
        std::string ip;
        uint32_t serial;

        uint8_t nbLights;
        DmxDeviceConf* lights;
    };

    KinetConfiguration(std::string filename);
    ~KinetConfiguration();

    bool load();

    std::string getInterface() {return interfaceKinet;}
    std::string getIpServer() {return ipServer;}

    uint8_t getNbPanels() {return nbPanels;}
    PanelConf* getPanel(int index);
    PanelConf* getPanel(std::string name);

    uint8_t getNbDmxControler() {return nbDmxControler;}
    DmxControlerConf* getDmxControler(int index);
    DmxControlerConf* getDmxControler(std::string name);

    std::string toString();

private:

    static DmxType TypeFromString(std::string typeString);

    std::string pathFile;

    // -- to interface with kinet devices
    std::string interfaceKinet;
    std::string ipServer;

    uint8_t nbPanels;
    PanelConf* panels;

    // -- normally only one but permit future expansion of dmx devices
    uint8_t nbDmxControler;
    DmxControlerConf* dmxs;
};
