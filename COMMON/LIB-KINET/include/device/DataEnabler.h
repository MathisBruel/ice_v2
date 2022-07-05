#pragma once

#include "device/Device.h"
#include "device/DmxDevice.h"

class DataEnabler : public Device
{

public:

    DataEnabler(std::string id, std::string name, std::string macAddress, std::string ip, int nbPorts);
    ~DataEnabler();
    void addDmxDevice(DmxDevice* dmx);
    void createPayload();
    void resetPayload();

    std::vector<DmxDevice*> getListDmxDevices() {return listDmxDevices;}

private:

    std::vector<DmxDevice*> listDmxDevices;
};