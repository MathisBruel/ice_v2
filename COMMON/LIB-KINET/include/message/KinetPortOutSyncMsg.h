#include "message/KinetMsg.h"

class KinetPortOutSyncMessage : public KinetMessage
{

public : 

    // -- constructor for creating and sending message
    KinetPortOutSyncMessage(KiNetVersion version, uint32_t sequenceNumber);
    // -- constructor for receiving and parsing message
    KinetPortOutSyncMessage(uint8_t* data, uint32_t length);
    ~KinetPortOutSyncMessage();

    // -- getters
    uint32_t getPad() {return pad;}
    uint8_t* getBytes();
    uint32_t getSize() {return Size;}

protected:
    static const int Size = 16;

private:

    // Not used, set to 0x00.
    uint32_t pad;

    void setData(uint8_t* data, uint32_t length);

};