#include "message/KinetMsg.h"

struct KinetDiscoveryPortsReplyEntry
{
    // Port number.
    uint8_t port;
    // Type of port.
    KinetMessage::KiNetPortTypeDiscoverPorts type;
    // Not used, set to 0x0000.
    uint16_t pad1;
    // Not used, set to 0x0000.
    uint32_t pad2;
};

class KinetDiscoveryPortReplyMessage : public KinetMessage
{

public : 

    // -- constructor for creating and sending message
    KinetDiscoveryPortReplyMessage(KiNetVersion version, uint32_t sequenceNumber, uint8_t numPorts, KinetDiscoveryPortsReplyEntry* portEntries);
    // -- constructor for receiving and parsing message
    KinetDiscoveryPortReplyMessage(uint8_t* data, uint32_t length);
    ~KinetDiscoveryPortReplyMessage();

    // -- getters
    uint8_t getNumPorts() {return numPorts;}
    uint8_t getPad1() {return pad1;}
    uint16_t getPad2() {return pad2;}
    KinetDiscoveryPortsReplyEntry* getEntries() {return portEntries;}
    uint8_t* getBytes();
    uint32_t getSize() {return Size+numPorts*sizeof(KinetDiscoveryPortsReplyEntry);}

protected:
    static const int Size = 16; // -- without strings

private:

    // Number of Port Entry blocks in the packet
    uint8_t numPorts;
    // Not used, set to 0x00.
    uint8_t pad1;
    // Not used, set to 0x0000.
    uint16_t pad2;

    // -- list of all entries
    KinetDiscoveryPortsReplyEntry* portEntries;


    void setData(uint8_t* data, uint32_t length);

};