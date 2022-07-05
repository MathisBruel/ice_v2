#include "device/DmxDevice.h"

DmxDevice::DmxDevice(std::string name, std::string id, uint32_t serial, uint16_t dmxOffset, std::string formatBuffer)
{
    this->name = name;
    this->id = id;
    this->serial = serial;
    this->dmxOffset = dmxOffset;
    this->bufferTemplate = formatBuffer;
    colorCorrection = nullptr;
    refColorCorrection = nullptr;
    type = UNKNOWN;

    payload = new uint8_t[bufferTemplate.length()];
    for (int i = 0; i < bufferTemplate.length() ; i++) {
        payload[i] = 0;
    }
}
    
DmxDevice::~DmxDevice()
{
    if (payload != nullptr) {
        delete payload;
    }
}