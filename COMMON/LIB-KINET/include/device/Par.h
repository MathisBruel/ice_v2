#pragma once

#include "device/DmxDevice.h"

class PAR : public DmxDevice
{
    #define DEFAULT_LUMENS_PAR (3500)

public:

    enum TypePAR
    {
        FEATURE,
        AMBIANCE,
        SCENE
    };

    PAR(std::string name, std::string id, uint32_t serial, uint16_t dmxOffset, std::string formatBuffer, TypePAR category, uint8_t defaultZoom, int lumens = DEFAULT_LUMENS_PAR);
    void createPayload(uint16_t red, uint16_t green, uint16_t blue, uint16_t white, uint8_t intensity, uint8_t zoom, bool lutApplied = true);
    void createPayload(uint16_t red, uint16_t green, uint16_t blue, uint16_t white, uint8_t intensity, bool lutApplied = true);

    TypePAR getCategory() {return category;}

    uint8_t getDefaultZoom() {return defaultZoom;}
    static int getDefaultLumens() {return DEFAULT_LUMENS_PAR;}
    double getRatioIntensity() {return (double)DEFAULT_LUMENS_PAR/(double)lumens;}

private:

    TypePAR category;
    uint8_t defaultZoom;
    int lumens;
};