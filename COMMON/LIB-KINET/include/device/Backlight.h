#pragma once

#include "device/DmxDevice.h"

class Backlight : public DmxDevice
{

public:

    Backlight(std::string name, std::string id, uint32_t serial, uint16_t dmxOffset, std::string formatBuffer);
    void createPayload(uint8_t red, uint8_t green, uint8_t blue, uint8_t amber = 0, bool lutApplied = true);

};