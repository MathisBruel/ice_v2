#include "message/KinetDiscoveryFixtureMsg.h"

KinetDiscoveryFixtureMessage::KinetDiscoveryFixtureMessage(KiNetVersion version, uint32_t sequenceNumber)
    : KinetMessage(version, KinetMessage::KiNetPacketType::DiscoverFixture, sequenceNumber)
{
    pad = 0;
}
KinetDiscoveryFixtureMessage::KinetDiscoveryFixtureMessage(uint8_t* data, uint32_t length)
{
    setData(data, length);
}
KinetDiscoveryFixtureMessage::~KinetDiscoveryFixtureMessage() {}

void KinetDiscoveryFixtureMessage::setData(uint8_t* data, uint32_t length)
{
    int offset = 0;
    ByteArray byteArray(data, length);

    KinetMessage::setData(data, length);
    offset += KinetMessage::getSize();
    pad = byteArray.getUInt32(offset, true);
}

uint8_t* KinetDiscoveryFixtureMessage::getBytes()
{
    uint8_t* header = KinetMessage::getBytes();
    ByteArray byteArray(Size);

    int offset = 0;
    byteArray.set(header, offset, KinetMessage::getSize());
    offset += KinetMessage::getSize();
    byteArray.setUInt32(pad, offset, true);

    delete[] header;

    return byteArray.getCopyData();
}