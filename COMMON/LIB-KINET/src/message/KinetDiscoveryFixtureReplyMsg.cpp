#include "message/KinetDiscoveryFixtureReplyMsg.h"

KinetDiscoveryFixtureReplyMessage::KinetDiscoveryFixtureReplyMessage(KiNetVersion version, uint32_t sequenceNumber, uint32_t serial)
    : KinetMessage(version, KinetMessage::KiNetPacketType::DiscoverFixtureReply, sequenceNumber)
{
    this->serial = serial;
}
KinetDiscoveryFixtureReplyMessage::KinetDiscoveryFixtureReplyMessage(uint8_t* data, uint32_t length)
{
    setData(data, length);
}
KinetDiscoveryFixtureReplyMessage::~KinetDiscoveryFixtureReplyMessage() {}

void KinetDiscoveryFixtureReplyMessage::setData(uint8_t* data, uint32_t length)
{
    int offset = 0;
    ByteArray byteArray(data, length);

    KinetMessage::setData(data, length);
    offset += KinetMessage::getSize();
    serial = byteArray.getUInt32(offset, true);
}

uint8_t* KinetDiscoveryFixtureReplyMessage::getBytes()
{
    uint8_t* header = KinetMessage::getBytes();
    ByteArray byteArray(Size);

    int offset = 0;
    byteArray.set(header, offset, KinetMessage::getSize());
    offset += KinetMessage::getSize();
    byteArray.setUInt32(serial, offset, true);

    delete[] header;

    return byteArray.getCopyData();
}