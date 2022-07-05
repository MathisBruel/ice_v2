#include "message/KinetDmxOut.h"

KinetDxmOutMessage::KinetDxmOutMessage(KiNetVersion version, uint32_t sequenceNumber, uint8_t port, uint8_t* payload, uint32_t length)
    : KinetMessage(version, KinetMessage::KiNetPacketType::DMXOut, sequenceNumber)
{
    this->port = port;
    this->payload = payload;
    universe = DefaultUniverse;
    sizePayload = length;
    flags = 0;
    timerVal = 0;
}
KinetDxmOutMessage::KinetDxmOutMessage(uint8_t* data, uint32_t length)
{
    setData(data, length);
}
KinetDxmOutMessage::~KinetDxmOutMessage() {}

void KinetDxmOutMessage::setData(uint8_t* data, uint32_t length)
{
    int offset = 0;
    ByteArray byteArray(data, length);

    KinetMessage::setData(data, length);
    offset += KinetMessage::getSize();
    port = byteArray.getUInt8(offset);
    offset += sizeof(uint8_t);
    flags = byteArray.getUInt8(offset);
    offset += sizeof(uint8_t);
    timerVal = byteArray.getUInt16(offset, true);
    offset += sizeof(uint16_t);
    universe = byteArray.getUInt32(offset, true);
    offset += sizeof(uint32_t);
    payload = byteArray.get(offset, length-offset);
    sizePayload = length-offset;
}

uint8_t* KinetDxmOutMessage::getBytes()
{
    uint8_t* header = KinetMessage::getBytes();
    ByteArray byteArray(Size+sizePayload);

    int offset = 0;
    byteArray.set(header, offset, KinetMessage::getSize());
    offset += KinetMessage::getSize();
    byteArray.setUInt8(port, offset);
    offset += sizeof(uint8_t);
    byteArray.setUInt8(flags, offset);
    offset += sizeof(uint8_t);
    byteArray.setUInt16(timerVal, offset, true);
    offset += sizeof(uint16_t);
    byteArray.setUInt32(universe, offset, true);
    offset += sizeof(uint32_t);
    byteArray.set(payload, offset, sizePayload);

    delete[] header;

    return byteArray.getCopyData();
}