#include "message/KinetMsg.h"

class KinetDiscoveryPortMessage : public KinetMessage
{

public : 

    // -- constructor for creating and sending message
    KinetDiscoveryPortMessage(KiNetVersion version, uint32_t sequenceNumber);
    // -- constructor for receiving and parsing message
    KinetDiscoveryPortMessage(uint8_t* data, uint32_t length);
    ~KinetDiscoveryPortMessage();

    // -- getters
    uint32_t getPad() {return pad;}
    uint8_t* getBytes();
    uint32_t getSize() {return Size;}

protected:
    static const int Size = 16;
    static const uint32_t DiscoverDummyIPDefault = 0xFECA000A;    // 10.0.202.254 (little endian)

private:

    // Not used, set to 0x00000000.
    uint32_t pad;

    void setData(uint8_t* data, uint32_t length);

};