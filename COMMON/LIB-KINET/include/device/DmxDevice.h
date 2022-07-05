#include <cstdlib>
#include <iostream>

#pragma once

#include "calibrer.h"
#include "image.h"

class DmxDevice
{

public: 

    enum DmxType {
        MOVING_HEAD,
        BACKLIGHT,
        PAR,
        UNKNOWN
    };

    uint16_t getDmxOffset() {return dmxOffset;}
    uint8_t getSizeBuffer() {return bufferTemplate.length();}
    std::string getName() {return name;}
    uint8_t* getPayload() {return payload;}
    DmxType getType() {return type;}
    std::string getBufferTemplate() {return bufferTemplate;}

    ~DmxDevice();
    void setRefColorCorrection(Calibrer* correction) {refColorCorrection = correction;}
    void setColorCorrection(std::string calibName, Calibrer* correction) {this->calibName = calibName; colorCorrection = correction;}
    std::string getCalibName() {return calibName;}
    
protected:

    DmxDevice();
    DmxDevice(std::string name, std::string id, uint32_t serial, uint16_t dmxOffset, std::string formatBuffer);

    std::string name;
    std::string id;
    DmxType type;
    uint32_t serial;
    uint16_t dmxOffset;
    std::string bufferTemplate;

    Calibrer* colorCorrection;
    std::string calibName;
    Calibrer* refColorCorrection;

    uint8_t* payload;
};