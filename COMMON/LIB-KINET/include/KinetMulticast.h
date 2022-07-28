#include <cstdlib>
#include <iostream>
#include <vector>

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Thread.h"
#include "Poco/Net/MulticastSocket.h"
#include "Poco/Net/NetworkInterface.h"
#include "message/KinetDiscoveryMsg.h"
#include "message/KinetDiscoveryPortMsg.h"
#include "message/KinetDiscoveryReplyMsg.h"
#include "message/KinetDiscoveryPortReplyMsg.h"
#include "message/KinetDiscoveryFixtureMsg.h"
#include "message/KinetDiscoveryFixtureReplyMsg.h"
#include "message/KinetDiscoveryChannelMsg.h"
#include "message/KinetDiscoveryChannelReplyMsg.h"
#include "message/KinetPortOutMsg.h"
#include "message/KinetPortOutSyncMsg.h"
#include "message/KinetDmxOut.h"
#include "Converter.h"
#include "portable_timer.h"


class KinetMulticast : public Poco::Runnable
{

static const uint16_t KinetPort = 0x1796;
static const uint16_t DiscoverClientPort = 0xF000;

public:

    KinetMulticast(std::string name, std::string interfaceName);
    KinetMulticast(std::string hostAddress);
    ~KinetMulticast();
    void setBroadcastAddress(std::string hostAddress);

    bool open();

    // -- discovery messages
    void sendDiscoveryMessage();
    void sendDiscoveryPortMessage(std::string address);
    void sendDiscoveryFixtureMessage(std::string address);
    void sendDiscoveryChannelMessage(std::string address, uint32_t serial, uint16_t magic);

    // -- update messages
    void sendPortOutMessage(std::string address, uint8_t port, uint8_t* payload, uint16_t length, KinetMessage::KiNetPortOutStartCode startCode);
    void sendPortOutSyncMessage(std::string address);
    void sendDmxOutMessage(std::string address, uint8_t* payload, uint32_t length);

    void stopThread() {stop = true;}

    std::vector<std::shared_ptr<KinetMessage>> getKinetMessages() {return kinetMessages;}
    void clearReplies() {kinetMessages.clear();}

protected:

    void run();

private:

    void handleResponse(uint8_t * buffer, uint32_t length);
    void setPrebuildMessagesBuffer();

    // -- prebuild messages
    uint8_t* portOutSyncBuffer;
    uint32_t sizePortOutSyncBuffer;
    uint8_t* discoveryBuffer;
    uint32_t sizeDiscoveryBuffer;
    uint8_t* discoveryPortBuffer;
    uint32_t sizeDiscoveryPortBuffer;
    uint8_t* discoveryFixtureBuffer;
    uint32_t sizeDiscoveryFixtureBuffer;

    // -- sockets
    Poco::Net::MulticastSocket* sender;

    // -- definition of route for sockets
	Poco::Net::SocketAddress localAddress;
    Poco::Net::SocketAddress broadcastAddress;
    Poco::Net::NetworkInterface interface;
    std::string interfaceName;

    // -- for retrying
    std::string addressRetry;

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

    std::vector<std::shared_ptr<KinetMessage>> kinetMessages;
};