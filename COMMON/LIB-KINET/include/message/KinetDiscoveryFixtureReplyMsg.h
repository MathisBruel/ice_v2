#include "message/KinetMsg.h"

class KinetDiscoveryFixtureReplyMessage : public KinetMessage
{

public : 

    // -- constructor for creating and sending message
    KinetDiscoveryFixtureReplyMessage(KiNetVersion version, uint32_t sequenceNumber, uint32_t serial);
    // -- constructor for receiving and parsing message
    KinetDiscoveryFixtureReplyMessage(uint8_t* data, uint32_t length);
    ~KinetDiscoveryFixtureReplyMessage();

    // -- getters
    uint32_t getSerial() {return serial;}
    uint8_t* getBytes();
    uint32_t getSize() {return Size;}

protected:
    static const int Size = 16;

private:
    uint32_t serial;

    void setData(uint8_t* data, uint32_t length);

};