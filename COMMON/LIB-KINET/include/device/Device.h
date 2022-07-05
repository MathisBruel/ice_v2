#include <cstdlib>
#include <iostream>

#pragma once

#include "lut.h"
#include "image.h"
#include "Poco/Logger.h"

class Device
{

public:

    enum TypeDevice {
        LUMINOUS_TEXTIL,
        DATA_ENABLER_PRO,
        UNKNOWN
    };

    Device() {}
    Device(std::string id, std::string name, std::string macAddress, std::string ip, int nbPorts);
    ~Device();

    // -- getters
    std::string getName() {return name;}
    TypeDevice getType() {return type;}
    std::string getIp() {return ip;}
    int getNbPorts() {return nbPorts;}
    uint8_t* getPayload(int portNumber);
    uint32_t getPayloadSize(int portNumber);

    void changeState(bool state) {activate = state;}
    bool isActivate() {return activate;}

    // -- for correction
    void setColorCorrection(std::string lutName, LUT* correction) {this->lutName = lutName; colorCorrection = correction;}
    std::string getLutName() {return lutName;}

protected:

    // -- essential to communicate with
    int nbPorts;
    std::string ip;

    // -- description fields
    std::string name;
    std::string id;
    std::string macAddress;
    TypeDevice type;

    // -- payloads constructed
    uint8_t** payloads;
    uint32_t* payloadsSize;

    // -- correction to be applied to device
    LUT* colorCorrection;
    std::string lutName;

    // -- return of discovery
    bool activate;
};