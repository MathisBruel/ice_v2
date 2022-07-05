#include "message/KinetMsg.h"

class KinetDxmOutMessage : public KinetMessage
{

public : 

    // -- constructor for creating and sending message
    KinetDxmOutMessage(KiNetVersion version, uint32_t sequenceNumber, uint8_t port, uint8_t* payload, uint32_t length);
    // -- constructor for receiving and parsing message
    KinetDxmOutMessage(uint8_t* data, uint32_t length);
    ~KinetDxmOutMessage();

    // -- getters
    uint8_t* getBytes();
    uint32_t getSize() {return Size+sizePayload;}

protected:
    static const int Size = 20; // -- size before payload
    static const uint32_t DefaultUniverse = 0xffffffff;    // -- no matching on universe

private:

    uint8_t port;
    uint8_t flags;
    uint16_t timerVal;
    // Each power/data supply has a universe setting to facilitate multicast data distribution. The
    // “don’t care” value for universe is -1 (0xFFFFFFFF).
    uint32_t universe;
    // defines the RDM message : first byte is DMX start code : 0xCC
    uint8_t* payload;
    uint32_t sizePayload;

    void setData(uint8_t* data, uint32_t length);

};