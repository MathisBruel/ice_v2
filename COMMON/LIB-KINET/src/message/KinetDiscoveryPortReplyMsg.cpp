#include "message/KinetDiscoveryPortReplyMsg.h"

KinetDiscoveryPortReplyMessage::KinetDiscoveryPortReplyMessage(KiNetVersion version, uint32_t sequenceNumber, uint8_t numPorts, 
    KinetDiscoveryPortsReplyEntry* portEntries)
: KinetMessage(version, KinetMessage::KiNetPacketType::DiscoverPortsReply, sequenceNumber)
{
    this->numPorts = numPorts;
    this->portEntries = portEntries;
}
KinetDiscoveryPortReplyMessage::KinetDiscoveryPortReplyMessage(uint8_t* data, uint32_t length)
{
    portEntries = nullptr;
    //Poco::Logger::get("KinetDiscoveryReplyMessage").debug("Parse data !");
    setData(data, length);
    //Poco::Logger::get("KinetDiscoveryReplyMessage").debug("Data parsed !");
}
KinetDiscoveryPortReplyMessage::~KinetDiscoveryPortReplyMessage() 
{   
    if (portEntries != nullptr) {
        delete[] portEntries;
    }
}

uint8_t* KinetDiscoveryPortReplyMessage::getBytes()
{
    uint8_t* header = KinetMessage::getBytes();
    ByteArray byteArray(Size+numPorts*sizeof(KinetDiscoveryPortsReplyEntry));

    int offset = 0;
    byteArray.set(header, offset, KinetMessage::getSize());
    offset += KinetMessage::getSize();
    byteArray.setUInt8(numPorts, offset);
    offset += sizeof(uint8_t);
    byteArray.setUInt8(pad1, offset);
    offset += sizeof(uint8_t);
    byteArray.setUInt16(pad2, offset, false);
    offset += sizeof(uint16_t);

    for (int i = 0; i < numPorts; i++) {

        byteArray.setUInt8(portEntries[i].port, offset);
        offset += sizeof(uint8_t);
        byteArray.setUInt8((uint8_t)portEntries[i].type, offset);
        offset += sizeof(uint8_t);
        byteArray.setUInt16(portEntries[i].pad1, offset, false);
        offset += sizeof(uint16_t);
        byteArray.setUInt16(portEntries[i].pad2, offset, false);
        offset += sizeof(uint32_t);
    }

    delete[] header;

    return byteArray.getCopyData();
}

void KinetDiscoveryPortReplyMessage::setData(uint8_t* data, uint32_t length)
{
    int offset = 0;
    ByteArray byteArray(data, length);

    KinetMessage::setData(data, length);
    offset += KinetMessage::getSize();
    numPorts = byteArray.getUInt8(offset);
    offset += sizeof(uint8_t);
    pad1 = byteArray.getUInt8(offset);
    offset += sizeof(uint8_t);
    pad2 = byteArray.getUInt16(offset, false);
    offset += sizeof(uint16_t);

    portEntries = new KinetDiscoveryPortsReplyEntry[numPorts];

    for (int i = 0; i < numPorts; i++) {
        portEntries[i].port = byteArray.getUInt8(offset);
        offset += sizeof(uint8_t);
        portEntries[i].type = (KiNetPortTypeDiscoverPorts)byteArray.getUInt8(offset);
        offset += sizeof(uint8_t);
        portEntries[i].pad1 = byteArray.getUInt16(offset, false);
        offset += sizeof(uint16_t);
        portEntries[i].pad2 = byteArray.getUInt32(offset, false);
        offset += sizeof(uint32_t);
    }
}