#include "device/Device.h"

Device::Device(std::string id, std::string name, std::string macAddress, std::string ip, int nbPorts)
{
    this->id = id;
    this->name = name;
    this->macAddress = macAddress;
    this->ip = ip;
    this->nbPorts = nbPorts;
    activate = true;
    colorCorrection = nullptr;
    type = UNKNOWN;
}

Device::~Device()
{
    // -- buffer are deleted when image are deleted
    if (payloads != nullptr) {
        delete payloads;
    }
    if (payloadsSize != nullptr) {
        delete payloadsSize;
    }
}

uint8_t* Device::getPayload(int portNumber)
{
    if (portNumber >= nbPorts || portNumber < 0) {
        Poco::Logger::get("Device").error("Port number " + std::to_string(portNumber) + " is invalid !", __FILE__, __LINE__);
        return nullptr;
    }

    return payloads[portNumber];
}

uint32_t Device::getPayloadSize(int portNumber)
{
    if (portNumber >= nbPorts || portNumber < 0) {
        Poco::Logger::get("Device").error("Port number " + std::to_string(portNumber) + " is invalid !", __FILE__, __LINE__);
        return 0;
    }

    return payloadsSize[portNumber];
}