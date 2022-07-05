#pragma once

#include "device/DmxDevice.h"

#define DEFAULT_LUMENS_MH (1750)

class MovingHead : public DmxDevice
{

public:

    MovingHead(std::string name, std::string id, uint32_t serial, uint16_t dmxOffset, std::string formatBuffer, 
    int panFullAngle, int tiltFullAngle, double biasPan, double biasTilt, uint8_t defaultZoom, int lumens = DEFAULT_LUMENS_MH);
    void createPayload(uint16_t red, uint16_t green, uint16_t blue, uint16_t white, uint8_t intensity, uint8_t zoom, double pan, double tilt, bool lutApplied = true);
    void createPayload(uint16_t red, uint16_t green, uint16_t blue, uint16_t white, uint8_t intensity, double pan, double tilt, bool lutApplied = true);

    uint8_t getDefaultZoom() {return defaultZoom;}
    static int getDefaultLumens() {return DEFAULT_LUMENS_MH;}
    double getRatioIntensity() {return (double)DEFAULT_LUMENS_MH/(double)lumens;}

private:

    int lumens;
    int panFullAngle;
    int tiltFullAngle;
    double biasPan;
    double biasTilt;
    uint8_t defaultZoom;
};