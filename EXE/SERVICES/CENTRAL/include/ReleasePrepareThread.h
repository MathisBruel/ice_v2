#include <cstdlib>
#include <iostream>
#include <queue>
#include <map>
#include <vector>
#include <regex>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "CentralContext.h"

class ReleasePrepareThread : public Poco::Runnable
{

public:

    ReleasePrepareThread();
    ~ReleasePrepareThread();
    void startThread();
    void stopThread() {stop = true;}

protected:

    void run();

private:

    std::shared_ptr<ScriptState> checkScriptAssociated(std::shared_ptr<Release> release);
    void updateAssociatedCpl(std::shared_ptr<Release> release);

    std::string loginServerICE(Poco::Net::HTTPClientSession* sessionICE);
    std::string getCplContent(Poco::Net::HTTPClientSession* sessionICE, std::string sessionId, std::string uuid);
    void logoutICE(Poco::Net::HTTPClientSession* sessionICE, std::string sessionId);

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

    // -- temporary datas
    std::vector<int> idCinemas;
    std::map<int, std::shared_ptr<Cinema>> cinemas;
};