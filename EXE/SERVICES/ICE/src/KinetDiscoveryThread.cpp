#include "KinetDiscoveryThread.h"

KinetDiscoveryThread::KinetDiscoveryThread()
{
    stop = false;
    thread = nullptr;
}
    
KinetDiscoveryThread::~KinetDiscoveryThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}

void KinetDiscoveryThread::startThread()
{
    // -- check all is initialized before launch
    KinetMulticast* kinet = ApplicationContext::getCurrentContext()->getKinet();
    if (kinet == nullptr) {
        Poco::Logger::get("KinetThread").error("kinet communicator is nullptr !");
        return;
    }

    // -- start thread
    thread = new Poco::Thread("KinetDiscovery");
    thread->setPriority(Poco::Thread::Priority::PRIO_LOW);
    thread->start(*this);
}

void KinetDiscoveryThread::stopThread()
{
    stop = true;
    if (!thread->tryJoin(2000)) {
        thread->sleep(5000);
        delete thread;
    }
}

void KinetDiscoveryThread::run() {

    // -- work every minute
    int waitTime = 60000;
    Poco::Stopwatch watch;
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    while (!stop) {
	    
        watch.reset();
        watch.start();

        // -- get datas from context
        KinetMulticast* kinet = context->getKinet();
        while (!context->tryLockKinetSenderMutex()) {usleep(20);}
        kinet->sendDiscoveryMessage();
        context->unlockKinetSenderMutex();

        // -- wait 30 seconds for all devices to reply
        usleep(30000000);

        // -- treat responses
        while (!context->tryLockListDevicesMutex()) {usleep(20);}
        std::map<std::string, Device*> listDevices(context->getListDevices());
        context->unlockListDevicesMutex();
        

        std::vector<std::shared_ptr<KinetMessage>> messages(kinet->getKinetMessages());
        std::map<std::string, Device*>::iterator itDevices;

        // -- tries to find response for each device : identification by name
        for (itDevices = listDevices.begin(); itDevices != listDevices.end(); itDevices++) {

            bool found = false;
            for (int i = 0; i < messages.size(); i++) {

                std::shared_ptr<KinetMessage> discoveryReply(messages.at(i));
                KinetDiscoveryReplyMessage* replyConverted = static_cast<KinetDiscoveryReplyMessage*>(discoveryReply.get());
                if ( itDevices->second->getIp() ==  Converter::ipToString(replyConverted->getIpAddress())) {
                    found = true;
                }
            }

            // -- we change only if difference (no need mutex)
            if (itDevices->second->isActivate() && !found) {
                while (!context->tryLockListDevicesMutex()) {usleep(20);}
                itDevices->second->changeState(false);
                context->unlockListDevicesMutex();
                Poco::Logger::get("KinetDiscoveryThread").error("Device " + itDevices->first + " not responding : deactivating !");
            }
            else if (!itDevices->second->isActivate() && found) {
                while (!context->tryLockListDevicesMutex()) {usleep(20);}
                itDevices->second->changeState(true);
                context->unlockListDevicesMutex();
                Poco::Logger::get("KinetDiscoveryThread").information("Device " + itDevices->first + " now responding : reactivating !");
            }
        }

        kinet->clearReplies();

        // -- wait for next send request
        int sleepTime = waitTime*1000-watch.elapsed();
        
        if (sleepTime > 0) {
            usleep(sleepTime);
        }
        else {
            Poco::Logger::get("KinetDiscoveryThread").warning("Thread exceeding time of " + std::to_string(-sleepTime) + "µs");
        }
    }
}