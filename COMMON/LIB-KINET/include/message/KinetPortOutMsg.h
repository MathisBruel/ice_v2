#include "message/KinetMsg.h"

class KinetPortOutMessage : public KinetMessage
{

public : 

    // -- constructor for creating and sending message
    KinetPortOutMessage(KiNetVersion version, uint32_t sequenceNumber, uint32_t universe, uint8_t port, 
                        KiNetPortOutFlags flags, KiNetPortOutStartCode startCode, 
                        uint16_t length, uint8_t* payload);
    // -- constructor for receiving and parsing message
    KinetPortOutMessage(uint8_t* data, uint32_t length);
    ~KinetPortOutMessage();

    // -- getters
    uint32_t getUniverse() {return universe;}
    uint8_t getPort() {return port;}
    uint8_t getPad() {return pad;}
    uint16_t getFlags() {return (uint16_t)flags;}
    uint16_t getLength() {return length;}
    uint16_t getStartCode() {return (uint16_t)startCode;}
    uint8_t* getPayload() {return payload;}
    uint8_t* getBytes();
    uint32_t getSize() {return Size+length;}

protected:
    static const int Size = 24; // -- without payload

private:

    // Each power/data supply has a universe setting to facilitate multicast data distribution. The
    uint32_t universe;
    // Specifies the port on the supply which the data is going to. Supplies may have anywhere from 1 to 16 ports.
    uint8_t port;
    // Not used, set to 0x00.
    uint8_t pad;
    // Structure of payload
    KiNetPortOutFlags flags;
    // Set to number of bytes in KiNET payload.
    uint16_t length;
    // Set to 0x0FFF for ChromASIC-based lights. Set to 0x0000 for non-CA lights.
    KiNetPortOutStartCode startCode;
    // Data is sent in RGB triples with one byte per color.
    uint8_t* payload;

    void setData(uint8_t* data, uint32_t length);

};