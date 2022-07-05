#include <cstdlib>
#include <iostream>
#include <map>

#pragma once
#include "Poco/Net/UDPClient.h"
#include "command.h"
#include "commandResponse.h"
#include "portable_timer.h"

class UdpCommandReceiver : public Poco::Runnable
{

public:

    UdpCommandReceiver(std::string address, uint16_t port);
    ~UdpCommandReceiver();

    void stopThread() {stop = true;}
    
    std::shared_ptr<Command> getFirstCommand();
    void deleteCommand(std::string uuid);
    void addCommand(std::shared_ptr<Command> command);

protected:

    void run();

private:

    void handleResponse(uint8_t * buffer, uint32_t length);

    // -- commands & responses
    std::map<std::string, std::shared_ptr<Command>> commands;
    Poco::Mutex commandsMutex;
    Poco::Net::DatagramSocket* receiver;

    Poco::Thread* thread;
	bool stop;
};