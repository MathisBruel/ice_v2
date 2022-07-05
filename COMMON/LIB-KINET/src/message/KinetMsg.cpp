#include "message/KinetMsg.h"

KinetMessage::KinetMessage()
{
    magicNumber = KinetMessage::KiNetMagic;
    this->version = version;
    this->type = type;
    this->sequenceNumber = sequenceNumber;
}

KinetMessage::KinetMessage(KiNetVersion version, KiNetPacketType type, uint32_t sequenceNumber)
{
    magicNumber = KinetMessage::KiNetMagic;
    this->version = version;
    this->type = type;
    this->sequenceNumber = sequenceNumber;
}
KinetMessage::KinetMessage(uint8_t* data, uint32_t length)
{
    setData(data, length);
}
KinetMessage::~KinetMessage() {}

void KinetMessage::setData(uint8_t* data, uint32_t length)
{
    ByteArray byteArray(data, length);
    int offset = 0;
    magicNumber = byteArray.getUInt32(offset, true);
    offset += sizeof(uint32_t);
    version = (KiNetVersion)byteArray.getUInt16(offset, true);
    offset += sizeof(uint16_t);
    type = (KiNetPacketType)byteArray.getUInt16(offset, true);
    offset += sizeof(uint16_t);
    sequenceNumber = byteArray.getUInt32(offset, true);
}

uint8_t* KinetMessage::getBytes()
{
    ByteArray byteArray(Size);
    int offset = 0;
    byteArray.setUInt32(magicNumber, offset, true);
    offset += sizeof(uint32_t);
    byteArray.setUInt16((uint16_t)version, offset, true);
    offset += sizeof(uint16_t);
    byteArray.setUInt16((uint16_t)type, offset, true);
    offset += sizeof(uint16_t);
    byteArray.setUInt32(sequenceNumber, offset, true);

    return byteArray.getCopyData();
}