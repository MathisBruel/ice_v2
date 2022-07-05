#include "message/KinetDiscoveryChannelMsg.h"

KinetDiscoveryChannelMessage::KinetDiscoveryChannelMessage(KiNetVersion version, uint32_t sequenceNumber, uint32_t serial, uint16_t magic)
    : KinetMessage(version, KinetMessage::KiNetPacketType::DiscoverChannel, magic)
{
    this->serial = serial;
    this->magic = magic;
    this->padding = 0;
}
KinetDiscoveryChannelMessage::KinetDiscoveryChannelMessage(uint8_t* data, uint32_t length)
{
    setData(data, length);
}
KinetDiscoveryChannelMessage::~KinetDiscoveryChannelMessage() {}

void KinetDiscoveryChannelMessage::setData(uint8_t* data, uint32_t length)
{
    int offset = 0;
    ByteArray byteArray(data, length);

    KinetMessage::setData(data, length);
    offset += KinetMessage::getSize();
    serial = byteArray.getUInt32(offset, true);
    offset += sizeof(uint32_t);
    magic = byteArray.getUInt16(offset, true);
    offset += sizeof(uint16_t);
    padding = byteArray.getUInt16(offset, true);
}

uint8_t* KinetDiscoveryChannelMessage::getBytes()
{
    uint8_t* header = KinetMessage::getBytes();
    ByteArray byteArray(Size);

    int offset = 0;
    byteArray.set(header, offset, KinetMessage::getSize());
    offset += KinetMessage::getSize();
    byteArray.setUInt32(serial, offset, true);
    offset += sizeof(uint32_t);
    byteArray.setUInt16(magic, offset, true);
    offset += sizeof(uint16_t);
    byteArray.setUInt16(padding, offset, true);

    delete[] header;

    return byteArray.getCopyData();
}