#include "message/KinetMsg.h"

class KinetDiscoveryMessage : public KinetMessage
{

public : 

    // -- constructor for creating and sending message
    KinetDiscoveryMessage(KiNetVersion version, uint32_t sequenceNumber);
    // -- constructor for receiving and parsing message
    KinetDiscoveryMessage(uint8_t* data, uint32_t length);
    ~KinetDiscoveryMessage();

    // -- getters
    uint32_t getIpAddress() {return tempIPAddress;}
    uint8_t* getBytes();
    uint32_t getSize() {return Size;}

protected:
    static const int Size = 16;
    static const uint32_t DiscoverDummyIPDefault = 0xFECA000A;    // 10.0.202.254 (little endian)

private:

    // Temporary IP address for discovery reply
    uint32_t tempIPAddress;

    void setData(uint8_t* data, uint32_t length);

};