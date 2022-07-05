#include "message/KinetMsg.h"

class KinetDiscoveryReplyMessage : public KinetMessage
{

public : 

    // -- constructor for creating and sending message
    KinetDiscoveryReplyMessage(KiNetVersion version, uint32_t sequenceNumber, uint32_t ipAddress, 
        uint8_t* macAddress, uint32_t serial, uint32_t universe, std::string deviceString, std::string idString);
    // -- constructor for receiving and parsing message
    KinetDiscoveryReplyMessage(uint8_t* data, uint32_t length);
    ~KinetDiscoveryReplyMessage();

    // -- getters
    uint32_t getIpAddress() {return ipAddress;}
    uint8_t* getMacAddress() {return macAddress;}
    uint32_t getSerial() {return serial;}
    uint32_t getUniverse() {return universe;}
    std::string getDevice() {return deviceString;}
    std::string getId() {return idString;}
    uint8_t* getBytes();
    uint32_t getSize() {return Size+deviceString.length()+idString.length();}

protected:
    static const int Size = 32; // -- without strings

private:

    // IP address of the supply issuing the discover reply packet.
    uint32_t ipAddress;
    // MAC address of the supply issuing the discover reply packet. (size 6)
    uint8_t* macAddress;
    // Supported KiNET version for the supply.
    KiNetVersion version;
    // Serial number of the supply issuing the discover reply packet.
    uint32_t serial;
    // Universe number of the supply issuing the discover reply packet. Universe numbers can be used when sending data to achieve multicast-like semantics.
    uint32_t universe;
    // Hard-coded device description string.
    std::string deviceString;
    // User-modifiable device identity string.
    std::string idString;

    void setData(uint8_t* data, uint32_t length);

};