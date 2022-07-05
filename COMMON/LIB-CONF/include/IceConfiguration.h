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

class IceConfiguration
{

public: 

    enum ProjectionSide {
        LEFT,
        RIGHT,
        UP,
        UNKNOW
    };

    struct ScreenConf {
        uint16_t width;
        uint16_t height;
        uint16_t offsetX;
        uint16_t offsetZ;
    };

    struct SeatConf {
        uint16_t offsetX;
        uint16_t offsetY;
        uint16_t offsetZ;
    };

    struct PanelConf {
        std::string name;
        ProjectionSide side;
        uint16_t offsetX;
        uint16_t offsetY;
        uint16_t offsetZ;
        uint16_t width;
        uint16_t height;
    };

    struct MovingheadConf {
        std::string name;
        bool left;
        bool front;
        uint16_t offsetX;
        uint16_t offsetY;
        uint16_t offsetZ;
        int panFullAngle;
        int tiltFullAngle;
        double biasPan;
        double biasTilt;
        double minX;
        double maxX;
        double minY;
        double maxY;
        double minZ;
        double maxZ;
    };

    IceConfiguration(std::string filename);
    ~IceConfiguration();

    bool load();

    ScreenConf* getScreen() {return screen;}
    SeatConf* getFrontLeftSeat() {return frontLeftSeat;}
    SeatConf* getRearRightSeat() {return rearRightSeat;}

    uint8_t getNbPanels() {return nbPanel;}
    PanelConf* getPanel(int index);
    PanelConf* getPanel(std::string name);

    uint8_t getNbMovingHeads() {return nbMovingHeads;}
    MovingheadConf* getMovingHead(int index);
    MovingheadConf* getMovingHead(std::string name);

    static std::string SideToString(ProjectionSide side);
    std::string toString();

private:

    std::string pathFile;
    static ProjectionSide SideFromString(std::string sideString);

    ScreenConf* screen;
    SeatConf* frontLeftSeat;
    SeatConf* rearRightSeat;

    uint8_t nbPanel;
    PanelConf* panels;

    uint8_t nbMovingHeads;
    MovingheadConf* movingHeads;
};