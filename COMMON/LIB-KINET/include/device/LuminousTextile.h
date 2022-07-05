#pragma once

#include "device/Device.h"
#include "Poco/Logger.h"
#include "Poco/Stopwatch.h"

class LuminousTextile : public Device
{

public:

    LuminousTextile(std::string id, std::string name, std::string macAddress, std::string ip, uint16_t nbPixels);
    ~LuminousTextile();

    void createPayload(std::shared_ptr<Image> img, bool lutApplied = true);
    void createPayload(uint8_t red, uint8_t green, uint8_t blue, bool lutApplied = true);

    void freeImg() {img.reset();}

    int getWidth() {return width;}
    int getHeight() {return height;}

private:

    int width;
    int height;
    int* heightPort;
    std::shared_ptr<Image> img;
};