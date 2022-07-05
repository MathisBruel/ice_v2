#include "PlayoutThread.h"

PlayoutThread::PlayoutThread(Device* device, std::shared_ptr<Image> img, bool lutApplied)
{
    running = true;
    this->img = img;
    this->device = device;
    this->lutApplied = lutApplied;
}
    
PlayoutThread::~PlayoutThread() {
    img.reset();
}

void PlayoutThread::run() {

    ApplicationContext* context = ApplicationContext::getCurrentContext();
    KinetMulticast* kinet = context->getKinet();

    if (device != nullptr) {

        // -- compute here payload (color correction may take some time // parallelism to DO)
        if (device->getType() == Device::LUMINOUS_TEXTIL && img != nullptr) {

            //Poco::Logger::get("PlayoutThread").debug("Create panel payload", __FILE__, __LINE__);
            LuminousTextile* panel =  static_cast<LuminousTextile*>(device);
            panel->createPayload(img, lutApplied);

            //Poco::Logger::get("PlayoutThread").debug("Send panel payload", __FILE__, __LINE__);
            for (int i = 0; i < panel->getNbPorts(); i++) {
                while (!context->tryLockKinetSenderMutex()) {usleep(20);}
                kinet->sendPortOutMessage(panel->getIp(), i+1, panel->getPayload(i), panel->getPayloadSize(i), KinetMessage::KiNetPortOutStartCode::StartCodeNonChromAsic);
                context->unlockKinetSenderMutex();
            }
            while (!context->tryLockKinetSenderMutex()) {usleep(20);}
            kinet->sendPortOutSyncMessage(panel->getIp());
            context->unlockKinetSenderMutex();

            //Poco::Logger::get("PlayoutThread").debug("Payload send", __FILE__, __LINE__);
            panel->freeImg();
        }

        else if (device->getType() == Device::DATA_ENABLER_PRO) {

            //Poco::Logger::get("PlayoutThread").debug("Create enabler payload", __FILE__, __LINE__);
            DataEnabler* enabler =  static_cast<DataEnabler*>(device);
            enabler->createPayload();

            //Poco::Logger::get("PlayoutThread").debug("Send enabler payload", __FILE__, __LINE__);
            for (int i = 0; i < enabler->getNbPorts(); i++) {
                for (int j = 0; j < 2; j++) {
                    while (!context->tryLockKinetSenderMutex()) {usleep(20);}
                    kinet->sendPortOutMessage(enabler->getIp(), i+1, enabler->getPayload(i), enabler->getPayloadSize(i), KinetMessage::KiNetPortOutStartCode::StartCodeNonChromAsic);
                    kinet->sendPortOutSyncMessage(enabler->getIp());
                    context->unlockKinetSenderMutex();
                    usleep(800);
                }
            }

            //Poco::Logger::get("PlayoutThread").debug("Payload send", __FILE__, __LINE__);
        }
    }

    img.reset();
    running = false;
}