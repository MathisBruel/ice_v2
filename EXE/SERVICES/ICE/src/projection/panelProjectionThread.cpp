#include "projection/panelProjectionThread.h"

PanelProjectionThread::PanelProjectionThread(std::string panelName, int currentFrame, Template* templateExtract)
{
    running = true;
    this->panelName = panelName;
    this->currentFrame = currentFrame;
    this->templateExtract = templateExtract;
}
    
PanelProjectionThread::~PanelProjectionThread() {}

void PanelProjectionThread::run() {

    ApplicationContext* context = ApplicationContext::getCurrentContext();

    while (!context->tryLockProjectionMutex()) {usleep(20);}
    PanelProjection::ProjectionType projection = context->getProjectionType();
    context->unlockProjectionMutex();

    // -- Define projector
    PanelProjection* projector = new PanelProjection(context->getConfig()->getIceConf(), context->getConfig()->getKinetConf(), templateExtract);
    if (!projector->projectPanel(panelName, projection, PanelProjection::MIX)) {
        Poco::Logger::get("PanelProjectionThread").error("Projection of panels " + panelName + "failed !", __FILE__, __LINE__);
    }
    else {
        context->insertPanelsOutput(currentFrame, projector->getOutput());
    }

    delete projector;
    running = false;
}