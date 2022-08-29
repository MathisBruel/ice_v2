#include "KinetMulticast.h"

KinetMulticast::KinetMulticast(std::string name, std::string interfaceName)
{
    stop = false;
    thread = nullptr;
    this->interfaceName = interfaceName;
}

KinetMulticast::KinetMulticast(std::string hostAddress)
{
    stop = false;
    thread = nullptr;
    localAddress = Poco::Net::SocketAddress(hostAddress, DiscoverClientPort);

    setPrebuildMessagesBuffer();
}

KinetMulticast::~KinetMulticast()
{
    if (sender != nullptr) {
        sender->close();
        delete sender;
    }

    stop = true;
	if (thread != nullptr)
	{
		thread->join();
		delete thread;
	}

    if (portOutSyncBuffer != nullptr) {
        delete[] portOutSyncBuffer;
    }
    if (discoveryBuffer != nullptr) {
        delete[] discoveryBuffer;
    }
    if (discoveryPortBuffer != nullptr) {
        delete[] discoveryPortBuffer;
    }
    if (discoveryFixtureBuffer != nullptr) {
        delete[] discoveryFixtureBuffer;
    }
}

void KinetMulticast::setBroadcastAddress(std::string hostAddress){
    broadcastAddress = Poco::Net::SocketAddress(hostAddress, KinetPort);
}

void KinetMulticast::setPrebuildMessagesBuffer()
{
    KinetPortOutSyncMessage msgPortOut(KinetMessage::KiNetVersion::Version2, 0);
    portOutSyncBuffer = msgPortOut.getBytes();
    sizePortOutSyncBuffer = msgPortOut.getSize();

    KinetDiscoveryMessage msgDiscovery(KinetMessage::KiNetVersion::Version2, 0);
    discoveryBuffer = msgDiscovery.getBytes();
    sizeDiscoveryBuffer = msgDiscovery.getSize();

    KinetDiscoveryPortMessage msgDiscoveryPort(KinetMessage::KiNetVersion::Version2, 0);
    discoveryPortBuffer = msgDiscoveryPort.getBytes();
    sizeDiscoveryPortBuffer = msgDiscoveryPort.getSize();

    KinetDiscoveryFixtureMessage msgDiscoveryFixture(KinetMessage::KiNetVersion::Version2, 0);
    discoveryFixtureBuffer = msgDiscoveryFixture.getBytes();
    sizeDiscoveryFixtureBuffer = msgDiscoveryFixture.getSize();
}

bool KinetMulticast::open()
{
    try {
        interface = Poco::Net::NetworkInterface::forName(interfaceName);
    } catch (std::exception &e) {
        Poco::Logger::get("KinetMulticast").error("Interface " + interfaceName + " not found !", __FILE__, __LINE__);
        return false;
    }

    localAddress = Poco::Net::SocketAddress(interface.address().toString(), DiscoverClientPort);
    setBroadcastAddress(interface.broadcastAddress().toString());
    setPrebuildMessagesBuffer();

    try {
        sender = new Poco::Net::MulticastSocket();
        sender->bind(localAddress, true);
        sender->setReusePort(true);
        sender->setBroadcast(true);
    } catch (std::exception &e) {
        Poco::Logger::get("KinetMulticast").error("Error while opening sender socket", __FILE__, __LINE__);
        return false; 
    }

    thread = new Poco::Thread();
	thread->start(*this);

    return true;
}

void KinetMulticast::sendDiscoveryMessage()
{
    int nbBytes = sender->sendTo(discoveryBuffer, sizeDiscoveryBuffer, broadcastAddress);

    if (nbBytes != sizeDiscoveryBuffer) {
        Poco::Logger::get("KinetMulticast").error("Send discovery : not all bytes have been sended out. Only " + std::to_string(nbBytes), __FILE__, __LINE__);
    }
}

void KinetMulticast::sendDiscoveryPortMessage(std::string address)
{
    int nbBytes = sender->sendTo(discoveryPortBuffer, sizeDiscoveryPortBuffer, Poco::Net::SocketAddress(address, KinetPort));

    if (nbBytes != sizeDiscoveryPortBuffer) {
        Poco::Logger::get("KinetMulticast").error("Send discovery port : not all bytes have been sended out. Only " + std::to_string(nbBytes), __FILE__, __LINE__);
    }
}

void KinetMulticast::sendDiscoveryFixtureMessage(std::string address)
{
    int nbBytes = sender->sendTo(discoveryFixtureBuffer, sizeDiscoveryFixtureBuffer, Poco::Net::SocketAddress(address, KinetPort));

    if (nbBytes != sizeDiscoveryFixtureBuffer) {
        Poco::Logger::get("KinetMulticast").error("Send discovery fixture : not all bytes have been sended out. Only " + std::to_string(nbBytes), __FILE__, __LINE__);
    }
}

void KinetMulticast::sendDiscoveryChannelMessage(std::string address, uint32_t serial, uint16_t magic)
{
    KinetDiscoveryChannelMessage msgDiscoveryChannel(KinetMessage::KiNetVersion::Version2, 0, serial, magic);

    uint8_t* buffer = msgDiscoveryChannel.getBytes();
    int nbBytes = sender->sendTo(buffer, msgDiscoveryChannel.getSize(), Poco::Net::SocketAddress(address, KinetPort));

    if (nbBytes != msgDiscoveryChannel.getSize()) {
        Poco::Logger::get("KinetMulticast").error("Send discovery channel : not all bytes have been sended out. Only " + std::to_string(nbBytes), __FILE__, __LINE__);
    }

    delete[] buffer;
}

void KinetMulticast::sendPortOutMessage(std::string address, uint8_t port, uint8_t* payload, uint16_t length, KinetMessage::KiNetPortOutStartCode startCode)
{
    KinetMessage::KiNetPortOutFlags flags = (KinetMessage::KiNetPortOutFlags)(
        (uint16_t)KinetMessage::KiNetPortOutFlags::Payload8bit | (uint16_t)KinetMessage::KiNetPortOutFlags::HoldForSync);

    KinetPortOutMessage msg(KinetMessage::KiNetVersion::Version2, (uint32_t)0, (uint32_t)0xFFFFFFFF, port, 
               flags, startCode, length, payload);

    uint8_t* buffer = msg.getBytes();

    int nbBytes = sender->sendTo(buffer, msg.getSize(), Poco::Net::SocketAddress(address, KinetPort));

    if (nbBytes != msg.getSize()) {
        Poco::Logger::get("KinetMulticast").error("Send port out : not all bytes have been sended out. Only " + std::to_string(nbBytes), __FILE__, __LINE__);
    }

    delete[] buffer;
}

void KinetMulticast::sendPortOutSyncMessage(std::string address)
{
    int nbBytes = sender->sendTo(portOutSyncBuffer, sizePortOutSyncBuffer, Poco::Net::SocketAddress(address, KinetPort));

    if (nbBytes != sizePortOutSyncBuffer) {
        Poco::Logger::get("KinetMulticast").error("Send port out sync : not all bytes have been sended out. Only " + std::to_string(nbBytes), __FILE__, __LINE__);
    }
}

void KinetMulticast::sendDmxOutMessage(std::string address, uint8_t* payload, uint32_t length)
{
    KinetDxmOutMessage msg(KinetMessage::KiNetVersion::Version1, (uint32_t)0, 0, payload, length);

    uint8_t* buffer = msg.getBytes();
    int nbBytes = sender->sendTo(msg.getBytes(), msg.getSize(), Poco::Net::SocketAddress(address, KinetPort));

    if (nbBytes != msg.getSize()) {
        Poco::Logger::get("KinetMulticast").error("Send DMX out : not all bytes have been sended out. Only " + std::to_string(nbBytes), __FILE__, __LINE__);
    }

    delete[] buffer;
}

void KinetMulticast::handleResponse(uint8_t * buffer, uint32_t length)
{
    // -- we parse the buffer received
    KinetMessage header((uint8_t*) buffer, length);
    if (header.getType() == KinetMessage::KiNetPacketType::DiscoverReply) {
        
        std::shared_ptr<KinetDiscoveryReplyMessage> discoveryReply = std::make_shared<KinetDiscoveryReplyMessage>(KinetDiscoveryReplyMessage((uint8_t*) buffer, length));
        kinetMessages.push_back(discoveryReply);
    } 
    else if (header.getType() == KinetMessage::KiNetPacketType::DiscoverPortsReply) {

        std::shared_ptr<KinetDiscoveryPortReplyMessage> discoveryPortReply = std::make_shared<KinetDiscoveryPortReplyMessage>(KinetDiscoveryPortReplyMessage((uint8_t*) buffer, length));
        kinetMessages.push_back(discoveryPortReply);
    }
    else if (header.getType() == KinetMessage::KiNetPacketType::DiscoverFixtureReply) {

        std::shared_ptr<KinetDiscoveryFixtureReplyMessage> discoveryFixtureReply = std::make_shared<KinetDiscoveryFixtureReplyMessage>(KinetDiscoveryFixtureReplyMessage((uint8_t*) buffer, length));

        kinetMessages.push_back(discoveryFixtureReply);
    }
    else if (header.getType() == KinetMessage::KiNetPacketType::DiscoverChannelReply) {

        std::shared_ptr<KinetDiscoveryChannelReplyMessage> discoveryChannelReply = std::make_shared<KinetDiscoveryChannelReplyMessage>(KinetDiscoveryChannelReplyMessage((uint8_t*) buffer, length));
        kinetMessages.push_back(discoveryChannelReply);
    }
    else {
        Poco::Logger::get("KinetMulticast").error("Unknow message type : " + std::to_string((uint16_t)header.getType()), __FILE__, __LINE__);
    }
}

void KinetMulticast::run()
{
    while (!stop) 
    {
        int available = sender->available();
        if (available > 0) {
            uint8_t* buffer = new uint8_t[available];
            int nbBytes = sender->receiveBytes(buffer, available);
            if (nbBytes > 0) {
                handleResponse(buffer, nbBytes);
            }
            delete[] buffer;
        }
        else {
            Timer::crossUsleep(5000);
        }
    }
}