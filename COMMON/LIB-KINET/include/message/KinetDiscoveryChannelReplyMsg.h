#include "message/KinetMsg.h"

class KinetDiscoveryChannelReplyMessage : public KinetMessage
{

public : 

    // -- constructor for creating and sending message
    KinetDiscoveryChannelReplyMessage(KiNetVersion version, uint32_t sequenceNumber, uint32_t serial, uint8_t channel, uint8_t ok);
    // -- constructor for receiving and parsing message
    KinetDiscoveryChannelReplyMessage(uint8_t* data, uint32_t length);
    ~KinetDiscoveryChannelReplyMessage();

    // -- getters
    uint32_t getSerial() {return serial;}
    uint16_t getMagic() {return magic;}
    uint8_t getChannel() {return channel;}
    uint8_t getOk() {return ok;}
    uint8_t* getBytes();
    uint32_t getSize() {return Size;}

protected:
    static const int Size = 20;
    static const int magicChannel = 0x96d6;

private:

    // -- serial of device concerned
    uint32_t serial;
    // -- magic sequence to be autorize to ask chennels
	uint16_t magic;
    uint8_t channel;
    uint8_t ok;

    void setData(uint8_t* data, uint32_t length);

};