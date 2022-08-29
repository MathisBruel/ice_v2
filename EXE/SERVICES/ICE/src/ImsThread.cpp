#include "ImsThread.h"

ImsThread::ImsThread()
{
    stop = false;
    thread = nullptr;
}
    
ImsThread::~ImsThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}

void ImsThread::startThread()
{
    // -- check all is initialized before launch
    ImsCommunicator* ims = ApplicationContext::getCurrentContext()->getIms();
    if (ims == nullptr) {
        Poco::Logger::get("ImsThread").error("Ims communicator is nullptr !", __FILE__, __LINE__);
        return;
    }

    ImsMessageHandler* handler = ims->getHandler();
    if (handler == nullptr) {
        Poco::Logger::get("ImsThread").error("Ims handler is nullptr !", __FILE__, __LINE__);
        return;
    }

    // -- login needed
    IMSConfiguration* imsConf = ApplicationContext::getCurrentContext()->getConfig()->getImsConf();
    std::shared_ptr<ImsRequest> loginRequest = std::shared_ptr<ImsRequest>(handler->createLoginRequest(imsConf->getUsername(), imsConf->getPassword()));
    std::shared_ptr<ImsResponse> response = std::make_shared<ImsResponse>(ImsResponse("", Command::UNKNOW_COMMAND));
    while (!ims->tryLock()) {usleep(20);}
    ims->sendRequest(loginRequest, response);
    ims->unlock();
    if (response->getStatus() == ImsResponse::KO) {
        Poco::Logger::get("ImsThread").error("Could not login to IMS !", __FILE__, __LINE__);
        return;
    }

    // -- wait for reception of response
    Poco::Thread::sleep(3);

    // -- check if loggedIn for thread to start
    thread = new Poco::Thread("Ims");
    thread->setPriority(Poco::Thread::Priority::PRIO_HIGHEST);
    thread->start(*this);
}

void ImsThread::run() {

    ApplicationContext* context = ApplicationContext::getCurrentContext();
    uint8_t waitTime = 10;
    Poco::Stopwatch watch;

    ImsCommunicator* ims = context->getIms();
    ImsMessageHandler* handler = ims->getHandler();

    while (!stop) {
	    
        watch.reset();
        watch.start();

        if (!handler->isLoggedIn()) {
            IMSConfiguration* imsConf = ApplicationContext::getCurrentContext()->getConfig()->getImsConf();
            std::shared_ptr<ImsRequest> loginRequest = std::shared_ptr<ImsRequest>(handler->createLoginRequest(imsConf->getUsername(), imsConf->getPassword()));
            std::shared_ptr<ImsResponse> response = std::make_shared<ImsResponse>(ImsResponse("", Command::UNKNOW_COMMAND));
            while (!ims->tryLock()) {usleep(20);}
            ims->sendRequest(loginRequest, response);
            ims->unlock();
            if (response->getStatus() == ImsResponse::KO) {
                Poco::Logger::get("ImsThread").error("Could not login to IMS !", __FILE__, __LINE__);
            }
        }

        if (handler->isLoggedIn()) {
            std::shared_ptr<ImsResponse> response = std::make_shared<ImsResponse>(ImsResponse("", Command::STATUS_IMS));
            std::shared_ptr<ImsRequest> playStatusRequest = std::shared_ptr<ImsRequest>(handler->createGetPlayerStatusRequest());

            if (ims->getProtocol() != ImsRequest::UDP) {
                while (!ims->tryLock()) {usleep(20);}
                ims->sendRequest(playStatusRequest, response);
                ims->unlock();
            }
            else {
                // -- STATE
                std::shared_ptr<ImsRequest> playStateRequest = std::shared_ptr<ImsRequest>(handler->createGetStatusItemRequest(ChristieHandler::PLAYBACK_STATE));
                while (!ims->tryLock()) {usleep(20);}
                ims->sendRequest(playStateRequest, response);
                ims->unlock();

                // -- TIMECODE
                std::shared_ptr<ImsRequest> playTimecodeRequest = std::shared_ptr<ImsRequest>(handler->createGetStatusItemRequest(ChristieHandler::PLAYBACK_TIMECODE));
                while (!ims->tryLock()) {usleep(20);}
                ims->sendRequest(playTimecodeRequest, response);
                ims->unlock();

                // -- CPLID
                std::shared_ptr<ImsRequest> playCplIdRequest = std::shared_ptr<ImsRequest>(handler->createGetStatusItemRequest(ChristieHandler::PLAYBACK_CPLID));
                while (!ims->tryLock()) {usleep(20);}
                ims->sendRequest(playCplIdRequest, response);
                ims->unlock();

                // -- CPLNAME
                std::shared_ptr<ImsRequest> playCplNameRequest = std::shared_ptr<ImsRequest>(handler->createGetStatusItemRequest(ChristieHandler::PLAYBACK_CPLNAME));
                while (!ims->tryLock()) {usleep(20);}
                ims->sendRequest(playCplNameRequest, response);
                ims->unlock();
            }

            if (response->getStatus() == ImsResponse::KO) {

                // -- error no new response
                Poco::Logger::get("ImsThread").warning("Response to get show status not received in time. Extrapolation needed !", __FILE__, __LINE__);

                // -- invalidate if too old (more than 500ms)
                while(!context->tryLockStatusPlayerMutex()) {usleep(20);}
                std::shared_ptr<PlayerStatus> status = context->getPlayerStatus();
                context->unlockStatusPlayerMutex();

                if (playStatusRequest->getTimestamp() - status->getTimestamp() > 500000) {
                    Poco::Logger::get("ImsThread").error("Response not received. Older one is too old : invalidate status !", __FILE__, __LINE__);

                    while(!context->tryLockStatusPlayerMutex()) {usleep(20);}
                    context->getPlayerStatus()->setState(PlayerStatus::UnknownState);
                    context->unlockStatusPlayerMutex();
                }
            }
            else {
                context->setPlayerStatus(response->getPlayerStatus());
            }

            int sleepTime = waitTime*1000-watch.elapsed();

            // -- wait for next send request
            if (sleepTime > 0) {
                usleep(sleepTime);
            }
            else {
                Poco::Logger::get("ImsThread").warning("Thread exceeding time of " + std::to_string(-sleepTime) + "µs", __FILE__, __LINE__);
            }
        }
    }

    // -- log out
    std::shared_ptr<ImsRequest> logout(handler->createLogoutRequest());
    std::shared_ptr<ImsResponse> response = std::make_shared<ImsResponse>(ImsResponse("", Command::UNKNOW_COMMAND));
    while (!ims->tryLock()) {usleep(20);}
    ims->sendRequest(logout, response);
    ims->unlock();
}