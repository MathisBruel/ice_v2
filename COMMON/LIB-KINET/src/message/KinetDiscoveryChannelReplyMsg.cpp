#include "message/KinetDiscoveryChannelReplyMsg.h"

KinetDiscoveryChannelReplyMessage::KinetDiscoveryChannelReplyMessage(KiNetVersion version, uint32_t sequenceNumber, uint32_t serial, uint8_t channel, uint8_t ok)
    : KinetMessage(version, KinetMessage::KiNetPacketType::DiscoverChannelReply, sequenceNumber)
{
    this->serial = serial;
    this->magic = magicChannel;
    this->channel = channel;
    this->ok = ok;
}
KinetDiscoveryChannelReplyMessage::KinetDiscoveryChannelReplyMessage(uint8_t* data, uint32_t length)
{
    setData(data, length);
}
KinetDiscoveryChannelReplyMessage::~KinetDiscoveryChannelReplyMessage() {}

void KinetDiscoveryChannelReplyMessage::setData(uint8_t* data, uint32_t length)
{
    int offset = 0;
    ByteArray byteArray(data, length);

    KinetMessage::setData(data, length);
    offset += KinetMessage::getSize();
    serial = byteArray.getUInt32(offset, true);
    offset += sizeof(uint32_t);
    magic = byteArray.getUInt16(offset, true);
    offset += sizeof(uint16_t);
    channel = byteArray.getUInt8(offset);
    offset += sizeof(uint8_t);
    ok = byteArray.getUInt8(offset);
    offset += sizeof(uint8_t);
}

uint8_t* KinetDiscoveryChannelReplyMessage::getBytes()
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
    byteArray.setUInt8(channel, offset);
    offset += sizeof(uint8_t);
    byteArray.setUInt8(ok, offset);
    offset += sizeof(uint8_t);

    delete[] header;

    return byteArray.getCopyData();
}