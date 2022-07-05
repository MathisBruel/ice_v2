#include "message/KinetMsg.h"

class KinetDiscoveryChannelMessage : public KinetMessage
{

public : 

    // -- constructor for creating and sending message
    KinetDiscoveryChannelMessage(KiNetVersion version, uint32_t sequenceNumber, uint32_t serial, uint16_t magic);
    // -- constructor for receiving and parsing message
    KinetDiscoveryChannelMessage(uint8_t* data, uint32_t length);
    ~KinetDiscoveryChannelMessage();

    // -- getters
    uint32_t getSerial() {return serial;}
    uint16_t getMagic() {return magic;}
    uint8_t* getBytes();
    uint32_t getSize() {return Size;}

protected:
    static const int Size = 20;

private:

    // -- serial of device concerned
    uint32_t serial;
    // -- magic sequence to recognize with response
	uint16_t magic;
    // -- not used set to 0
	uint16_t padding;

    void setData(uint8_t* data, uint32_t length);

};