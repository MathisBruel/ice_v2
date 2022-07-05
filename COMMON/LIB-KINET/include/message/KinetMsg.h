#include <cstdlib>
#include <iostream>

#pragma once
#include "ByteArray.h"

class KinetMessage
{

public:

    // -- definition of enum version
    enum class KiNetVersion : uint16_t {
        Version1 = 0x0001,
        Version2 = 0x0002
    };

    // -- definition of enum type Kinet message
    enum class KiNetPacketType : uint16_t {
        Discover = 0x0001,
        DiscoverReply = 0x0002,
        DiscoverPorts = 0x000A,
        DiscoverPortsReply = 0x000B,
        DMXOut = 0x0101,
        PortOut = 0x0108,
        PortOutSync = 0x0109,
        DiscoverFixture = 0x0201,
        DiscoverFixtureReply = 0x0202,
        DiscoverChannel = 0x0203,
        DiscoverChannelReply = 0x0204
    };

    // -- how payload is structured bitwise OR
    // •    0x01 – undefined
    // •    0x02 – payload format: 0=8 bit data, 1=16 bit data
    // •    0x04 – data sending: 0=send immediately, 1=hold for SYNC packet
    enum class KiNetPortOutFlags : uint16_t
    {
        Payload8bit = 0x0000,
        Payload16bit = 0x0002,
        SendImmediate = 0x0000,
        HoldForSync = 0x0004
    };

    // -- type of device
    enum class KiNetPortOutStartCode : uint16_t
    {
        StartCodeChromAsic = 0x0FFF,
        StartCodeNonChromAsic = 0x0000
    };

    // -- type du port
    enum class KiNetPortType
    {
        NullPort,
        SerialPort,
        DefaultPort,
        ChromAsicPort,
    };

    // -- type port kinet
    enum class KiNetPortTypeDiscoverPorts : uint8_t
    {
        NullPort = 0x00,
        DMXPortBlinkScan = 0x01,
        DMXPort = 0x02,
        ChromAsic = 0x03,
    };

    // -- constructor for creating and sending message
    KinetMessage();
    KinetMessage(KiNetVersion version, KiNetPacketType type, uint32_t sequenceNumber);
    // -- constructor for receiving and parsing message
    KinetMessage(uint8_t* data, uint32_t length);
    ~KinetMessage();

    // -- getters
    KiNetVersion getVersion() {return version;}
    KiNetPacketType getType() {return type;}
    uint32_t getSequenceNumber() {return sequenceNumber;}
    uint8_t* getBytes();
    uint32_t getSize() {return Size;}

protected:
    static const uint32_t KiNetMagic = 0x4ADC0104;
    static const int Size = 12;

    void setData(uint8_t* data, uint32_t length);
    
private:

    // KiNet communication is recognized when magic number is equal to 0x4ADC0104.
    uint32_t magicNumber;
    // Latest version of KiNET protocol is V2.0. Older supplies may support KiNET V1.0 only.
    KiNetVersion version;
    // Set to KiNet packet type.
    KiNetPacketType type;
    // Can be used for ordering/numbering of packets. Not implemented on most supplies. Should be set to 0.
    uint32_t sequenceNumber;

};