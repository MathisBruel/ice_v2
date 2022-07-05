#include "message/KinetMsg.h"

class KinetDiscoveryFixtureMessage : public KinetMessage
{

public : 

    // -- constructor for creating and sending message
    KinetDiscoveryFixtureMessage(KiNetVersion version, uint32_t sequenceNumber);
    // -- constructor for receiving and parsing message
    KinetDiscoveryFixtureMessage(uint8_t* data, uint32_t length);
    ~KinetDiscoveryFixtureMessage();

    // -- getters
    uint32_t getPad() {return pad;}
    uint8_t* getBytes();
    uint32_t getSize() {return Size;}

protected:
    static const int Size = 16;

private:

    // Not used, set to 0x00000000.
    uint32_t pad;

    void setData(uint8_t* data, uint32_t length);

};