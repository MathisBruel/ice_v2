#include "CommandDispatcherThread.h"

CommandDispatcherThread::CommandDispatcherThread(ImsCmdThread* imsThread, KinetThread* kinetThread, RepositoryCommandThread* repoCmdThread, 
                                                 ContextThread* contextThread, RendererThread* renderThread)
{
    this->imsThread = imsThread;
    this->kinetThread = kinetThread;
    this->repoCmdThread = repoCmdThread;
    this->contextThread = contextThread;
    this->renderThread = renderThread;
    stop = false;
    thread = nullptr;
}
    
CommandDispatcherThread::~CommandDispatcherThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}

void CommandDispatcherThread::startThread()
{
    // -- start thread
    thread = new Poco::Thread("CommandDispatcher");
    thread->setPriority(Poco::Thread::Priority::PRIO_HIGH);
    thread->start(*this);
}

void CommandDispatcherThread::run() {

    // -- work every 20 ms
    int waitTime = 20;
    Poco::Stopwatch watch;
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    while (!stop) {
	    
        watch.reset();
        watch.start();

        // -- try acquire mutex on commands & responses
        // -- wait for next send request
        std::shared_ptr<Command> cmd(context->getCommandHandler()->getFirstCommand());
        std::shared_ptr<Command> cmdUdp(context->getCommandUdpReceiver()->getFirstCommand());
        std::shared_ptr<Image> imgExternal(context->getStreamReceiver()->getLastAddedImage());
        context->getStreamReceiver()->removeLastAddedImage();
        int currentFrame = context->getStreamReceiver()->getCurrentFrame();

        // -- get player state
        while (!context->tryLockStatusPlayerMutex()) {usleep(20);}
        ApplicationContext::PlayerState state = context->getPlayerState();
        context->unlockStatusPlayerMutex();

        while (cmd != nullptr || cmdUdp != nullptr || imgExternal != nullptr) {

            if (cmdUdp != nullptr) {
                if (cmdUdp->getDispatch() == Command::CommandDispatch::IMS) {
                    //Poco::Logger::get("CommandDispatcherThread").debug("Dispatching to IMS", __FILE__, __LINE__);
                    imsThread->addCommand(cmdUdp);
                }
                else if (cmdUdp->getDispatch() == Command::CommandDispatch::KINET) {
                    //Poco::Logger::get("CommandDispatcherThread").debug("Dispatching to KINET", __FILE__, __LINE__);
                    kinetThread->addCommand(cmdUdp);
                }
                else if (cmdUdp->getDispatch() == Command::CommandDispatch::REPOSITORY) {
                    //Poco::Logger::get("CommandDispatcherThread").debug("Dispatching to REPOSITORY", __FILE__, __LINE__);
                    repoCmdThread->addCommand(cmdUdp);
                }
                else if (cmdUdp->getDispatch() == Command::CommandDispatch::CONTEXT) {
                    //Poco::Logger::get("CommandDispatcherThread").debug("Dispatching to CONTEXT", __FILE__, __LINE__);
                    contextThread->addCommand(cmdUdp);
                }
                else if (cmdUdp->getDispatch() == Command::CommandDispatch::PLAYER) {
                    //Poco::Logger::get("CommandDispatcherThread").debug("Dispatching to PLAYER", __FILE__, __LINE__);
                    renderThread->addCommand(cmdUdp);
                }

                // -- we remove command from map
                context->getCommandUdpReceiver()->deleteCommand(cmdUdp->getUuid());
                cmdUdp = std::shared_ptr<Command>(context->getCommandUdpReceiver()->getFirstCommand());
            }

            if (cmd != nullptr) {
                if (cmd->getDispatch() == Command::CommandDispatch::IMS) {
                    //Poco::Logger::get("CommandDispatcherThread").debug("Dispatching to IMS", __FILE__, __LINE__);
                    imsThread->addCommand(cmd);
                }
                else if (cmd->getDispatch() == Command::CommandDispatch::KINET) {
                    //Poco::Logger::get("CommandDispatcherThread").debug("Dispatching to KINET", __FILE__, __LINE__);
                    kinetThread->addCommand(cmd);
                }
                else if (cmd->getDispatch() == Command::CommandDispatch::REPOSITORY) {
                    //Poco::Logger::get("CommandDispatcherThread").debug("Dispatching to REPOSITORY", __FILE__, __LINE__);
                    repoCmdThread->addCommand(cmd);
                }
                else if (cmd->getDispatch() == Command::CommandDispatch::CONTEXT) {
                    //Poco::Logger::get("CommandDispatcherThread").debug("Dispatching to CONTEXT", __FILE__, __LINE__);
                    contextThread->addCommand(cmd);
                }
                else if (cmd->getDispatch() == Command::CommandDispatch::PLAYER) {
                    //Poco::Logger::get("CommandDispatcherThread").debug("Dispatching to PLAYER", __FILE__, __LINE__);
                    renderThread->addCommand(cmd);
                }

                // -- we remove command from map
                context->getCommandHandler()->deleteCommand(cmd->getUuid());
                cmd = std::shared_ptr<Command>(context->getCommandHandler()->getFirstCommand());
            }

            if (imgExternal != nullptr && state == ApplicationContext::EXTERNAL) {

                //Poco::Logger::get("CommandDispatcherThread").debug("Dispatching external to PLAYER", __FILE__, __LINE__);
                std::shared_ptr<Command> cmdExternal = std::make_shared<Command>(Command::PLAY_TEMPLATE_IMAGE);
                cmdExternal->addParameter("version", std::to_string(context->getStreamReceiver()->getVersion()));
                cmdExternal->addParameter("frame", std::to_string(currentFrame));
                cmdExternal->setImage(imgExternal);
                renderThread->addCommand(cmdExternal);
                imgExternal.reset();
            }
        }

        // -- erase no more used response
        context->getCommandHandler()->removeOldResponses();

        long sleepTime = waitTime*1000-watch.elapsed();

        if (sleepTime > 0) {
            usleep(sleepTime);
        }
        else {
            Poco::Logger::get("CommandDispatcherThread").warning("Thread exceeding time of " + std::to_string(-sleepTime) + "µs", __FILE__, __LINE__);
        }
    }
}