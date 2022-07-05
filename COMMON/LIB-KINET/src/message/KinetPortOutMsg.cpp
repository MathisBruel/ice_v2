#include "message/KinetPortOutMsg.h"

KinetPortOutMessage::KinetPortOutMessage(KinetMessage::KiNetVersion version, uint32_t sequenceNumber, uint32_t universe, uint8_t port, 
                        KinetMessage::KiNetPortOutFlags flags, KinetMessage::KiNetPortOutStartCode startCode, 
                        uint16_t length, uint8_t* payload)
: KinetMessage(version, KinetMessage::KiNetPacketType::PortOut, sequenceNumber)
{
    this->universe = universe;
    this->port = port;
    this->flags = flags;
    this->startCode = startCode;
    this->length = length;
    this->payload = payload;
    pad = 0;
}


KinetPortOutMessage::KinetPortOutMessage(uint8_t* data, uint32_t length)
{
    setData(data, length);
}
KinetPortOutMessage::~KinetPortOutMessage() {}

void KinetPortOutMessage::setData(uint8_t* data, uint32_t length)
{
    int offset = 0;
    ByteArray byteArray(data, length);

    KinetMessage::setData(data, length);
    offset += KinetMessage::getSize();
    universe = byteArray.getUInt32(offset, true);
    offset += sizeof(uint32_t);
    port = byteArray.getUInt8(offset);
    offset += sizeof(uint8_t);
    pad = byteArray.getUInt8(offset);
    offset += sizeof(uint8_t);
    flags = (KiNetPortOutFlags)byteArray.getUInt16(offset, true);
    offset += sizeof(uint16_t);
    length = byteArray.getUInt16(offset, true);
    offset += sizeof(uint16_t);
    startCode = (KiNetPortOutStartCode)byteArray.getUInt16(offset, true);
    offset += sizeof(uint16_t);
    payload = byteArray.get(offset, length);
}

uint8_t* KinetPortOutMessage::getBytes()
{
    uint8_t* header = KinetMessage::getBytes();
    ByteArray byteArray(Size+length);

    int offset = 0;
    byteArray.set(header, offset, KinetMessage::getSize());
    offset += KinetMessage::getSize();
    byteArray.setUInt32(universe, offset, true);
    offset += sizeof(uint32_t);
    byteArray.setUInt8(port, offset);
    offset += sizeof(uint8_t);
    byteArray.setUInt8(pad, offset);
    offset += sizeof(uint8_t);
    byteArray.setUInt16((uint16_t)flags, offset, true);
    offset += sizeof(uint16_t);
    byteArray.setUInt16(length, offset, true);
    offset += sizeof(uint16_t);
    byteArray.setUInt16((uint16_t)startCode, offset, true);
    offset += sizeof(uint16_t);
    byteArray.set(payload, offset, length);

    delete[] header;

    return byteArray.getCopyData();
}