#include "message/KinetDiscoveryReplyMsg.h"

KinetDiscoveryReplyMessage::KinetDiscoveryReplyMessage(KiNetVersion version, uint32_t sequenceNumber, uint32_t ipAddress, 
        uint8_t* macAddress, uint32_t serial, uint32_t universe, std::string deviceString, std::string idString)
    : KinetMessage(version, KinetMessage::KiNetPacketType::DiscoverReply, sequenceNumber)
{
    this->ipAddress = ipAddress;
    this->macAddress = macAddress;
    this->serial = serial;
    this->universe = universe;
    this->deviceString = deviceString;
    this->idString = idString;
}
KinetDiscoveryReplyMessage::KinetDiscoveryReplyMessage(uint8_t* data, uint32_t length)
{
    macAddress = nullptr;
    setData(data, length);
}
KinetDiscoveryReplyMessage::~KinetDiscoveryReplyMessage() 
{
    
}

void KinetDiscoveryReplyMessage::setData(uint8_t* data, uint32_t length)
{
    int offset = 0;
    ByteArray byteArray(data, length);

    KinetMessage::setData(data, length);
    offset += KinetMessage::getSize();
    ipAddress = byteArray.getUInt32(offset, false);
    offset += sizeof(uint32_t);
    //macAddress = byteArray.get(offset, 6);
    offset += 6;
    version = (KiNetVersion)byteArray.getUInt16(offset, false);
    offset += sizeof(uint16_t);
    serial = byteArray.getUInt32(offset, true);
    offset += sizeof(uint32_t);
    universe = byteArray.getUInt32(offset, true);
    offset += sizeof(uint32_t);
    deviceString = "";
    idString = "";
    while ((offset < length) && (byteArray.getData()[offset] != 0))
    {
        uint8_t *charData = byteArray.get(offset, 1);
        deviceString += (char)charData[0];
        offset++;
        delete[] charData;
    }
    offset++;
    while ((offset < length) && (byteArray.getData()[offset] != 0))
    {
        uint8_t *charData = byteArray.get(offset, 1);
        idString += (char)charData[0];
        offset++;
        delete[] charData;
    }

    //Poco::Logger::get("KinetDiscoveryReplyMessage").debug(deviceString + " : " + idString);
}

uint8_t* KinetDiscoveryReplyMessage::getBytes()
{
    uint8_t* header = KinetMessage::getBytes();
    ByteArray byteArray(Size+deviceString.length()+idString.length());

    int offset = 0;
    byteArray.set(header, offset, KinetMessage::getSize());
    offset += KinetMessage::getSize();
    byteArray.setUInt32(ipAddress, offset, false);
    offset += sizeof(uint32_t);
    byteArray.set(macAddress, offset, 6);
    offset += 6;
    byteArray.setUInt16((uint16_t)version, offset, false);
    offset += sizeof(uint16_t);
    byteArray.setUInt32(serial, offset, true);
    offset += sizeof(uint32_t);
    byteArray.setUInt32(universe, offset, true);
    offset += sizeof(uint32_t);
    byteArray.set((uint8_t*)deviceString.c_str(), offset, deviceString.length());
    offset += deviceString.length();
    byteArray.set((uint8_t*)idString.c_str(), offset, idString.length());

    delete[] header;

    return byteArray.getCopyData();
}