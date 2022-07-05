#include "device/DataEnabler.h"

DataEnabler::DataEnabler(std::string id, std::string name, std::string macAddress, std::string ip, int nbPorts)
    : Device(id, name, macAddress, ip, nbPorts)
{
    type = DATA_ENABLER_PRO;

    payloadsSize = new uint32_t[nbPorts];
    payloadsSize[0] = 512;
    
    payloads = new uint8_t*[nbPorts];
    payloads[0] = new uint8_t[payloadsSize[0]];
}

DataEnabler::~DataEnabler() 
{
    for (int i = 0; i < listDmxDevices.size() ; i++) {
        if (listDmxDevices[i] != nullptr) {
            delete listDmxDevices[i];
        }
    }

    delete[] payloads[0];
}

void DataEnabler::addDmxDevice(DmxDevice* dmx)
{
    listDmxDevices.push_back(dmx);
}
    
void DataEnabler::createPayload()
{
    // -- reset all values
    resetPayload();

    // -- parsing all dmx devices
    for (DmxDevice* dmx : listDmxDevices) {

        // -- fill final payload by the payload of each dmx devices at the associated offset
        for (int i = 0; i < dmx->getSizeBuffer(); i++) {
            payloads[0][i+dmx->getDmxOffset()] = dmx->getPayload()[i];
        }
    }
}

void DataEnabler::resetPayload()
{
    // -- reset all values
    for (int i = 0; i < payloadsSize[0]; i++) {
        payloads[0][i] = 0;
    }
}