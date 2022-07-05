#include <cstdlib>
#include <iostream>
#include <queue>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "ApplicationContext.h"
#include "command.h"
#include "commandResponse.h"
#include "projection/panelProjectionThread.h"
#include "projection/movingHeadProjectionThread.h"
#include "extraction/extractorThread.h"
#include "Poco/ThreadPool.h"

class RendererThread : public Poco::Runnable
{

public:

    RendererThread();
    ~RendererThread();
    void startThread();
    void stopThread() {stop = true;}
    void addCommand(std::shared_ptr<Command> cmd);

protected:

    void run();
    void executeCommand(std::shared_ptr<Command> cmd);

private:

    PanelProjectionThread* findAvailableThread();
    bool panelProjectionThreadStillRunning();

    Poco::Mutex commandsMutex;
    std::map<std::string, std::shared_ptr<Command>> commands;
    void handlePlayTemplateImage(Command* cmd, CommandResponse* response);
    void playBlackOnAllDevices(int currentFrame);

    PanelProjectionThread** pool;
    Poco::ThreadPool* poolOfThread;

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

};