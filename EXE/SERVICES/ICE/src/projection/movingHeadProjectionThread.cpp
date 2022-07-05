#include "projection/movingHeadProjectionThread.h"

MovingHeadProjectionThread::MovingHeadProjectionThread(int currentFrame, Template* extractor)
{
    this->currentFrame = currentFrame;
    this->extractor = extractor;
    running = true;
}
    
MovingHeadProjectionThread::~MovingHeadProjectionThread()
{
}

void MovingHeadProjectionThread::run() {

    ApplicationContext* context = ApplicationContext::getCurrentContext();
    MovingHeadProjection *projector = context->getMovingHeadProjector();
    projector->setTemplateExtractor(extractor);

    if (projector != nullptr && extractor != nullptr) {
        if (!projector->projectMovingHeads()) {
            Poco::Logger::get("MovingHeadProjectionThread").error("Projection of moving heads failed !", __FILE__, __LINE__);
            running = false;
            return;
        }
        else {
            std::shared_ptr<MovingHeadProjection::OutputMovingHeadAngle> output(projector->getOutput());
            while (output != nullptr) {
                context->insertMovingHeadOutput(currentFrame, output);
                output.reset();
                output = projector->getOutput();
            }
            projector->clean();
        }
    }
    else {
        Poco::Logger::get("MovingHeadProjectionThread").error("Projector of moving heads is nullptr !", __FILE__, __LINE__);
    }

    running = false;
}