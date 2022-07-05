#include "message/KinetDiscoveryMsg.h"

KinetDiscoveryMessage::KinetDiscoveryMessage(KiNetVersion version, uint32_t sequenceNumber)
    : KinetMessage(version, KinetMessage::KiNetPacketType::Discover, sequenceNumber)
{
    tempIPAddress = DiscoverDummyIPDefault;
}
KinetDiscoveryMessage::KinetDiscoveryMessage(uint8_t* data, uint32_t length)
{
    setData(data, length);
}
KinetDiscoveryMessage::~KinetDiscoveryMessage() {}

void KinetDiscoveryMessage::setData(uint8_t* data, uint32_t length)
{
    int offset = 0;
    ByteArray byteArray(data, length);

    KinetMessage::setData(data, length);
    offset += KinetMessage::getSize();
    tempIPAddress = byteArray.getUInt32(offset, true);
}

uint8_t* KinetDiscoveryMessage::getBytes()
{
    uint8_t* header = KinetMessage::getBytes();
    ByteArray byteArray(Size);

    int offset = 0;
    byteArray.set(header, offset, KinetMessage::getSize());
    offset += KinetMessage::getSize();
    byteArray.setUInt32(tempIPAddress, offset, true);

    delete[] header;

    return byteArray.getCopyData();
}