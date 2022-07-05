#include <cstdlib>
#include <iostream>
#include <queue>
#include <map>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "CentralContext.h"
#include "ScriptInfoFile.h"

// -- IMS connector
#include "ImsCommunicator.h"
#include "ImsMessageHandler.h"
#include "ImsRequest.h"
#include "ImsResponse.h"
#include "device/BarcoHandler.h"
#include "device/ChristieHandler.h"
#include "device/DoremiHandler.h"

// -- ICE session
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPResponse.h"

#include "Data/Auditorium.h"
#include "Data/Server.h"
#include "StateServer.h"

class StateServerThread : public Poco::Runnable
{

public:

    StateServerThread();
    ~StateServerThread();
    void startThread();
    void stopThread() {stop = true;}

protected:

    void run();

private:

    std::shared_ptr<StateServer> checkResponseIms(std::shared_ptr<Auditorium> auditorium);
    std::shared_ptr<StateServer> checkServer(std::shared_ptr<Server> server, bool checkListCpls = false);

    // -- login / logout
    std::string loginServerICE(Poco::Net::HTTPClientSession* sessionICE);
    void logoutICE(Poco::Net::HTTPClientSession* sessionICE, std::string sessionId);

    // -- get infos from server
    std::vector<std::string> checkDeviceICE(Poco::Net::HTTPClientSession* sessionICE, std::string sessionId);
    std::vector<std::shared_ptr<ScriptInfoFile>> getScriptsICE(Poco::Net::HTTPClientSession* sessionICE, std::string sessionId);
    std::vector<std::shared_ptr<Cpl>> getListCpls(Poco::Net::HTTPClientSession* sessionICE, std::string sessionId);

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

};