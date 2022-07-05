#include "extraction/extractorThread.h"

ExtractorThread::ExtractorThread(TypeExtraction type, Template* extractor)
{
    this->type = type;
    this->extractor = extractor;
    running = true;
}
    
ExtractorThread::~ExtractorThread()
{

}

void ExtractorThread::run() {

    ApplicationContext* context = ApplicationContext::getCurrentContext();
    if (extractor != nullptr) {
        if (type == MOVING_HEAD) {
            extractor->extractMovingHeadsDatas();
        }
        else if (type == PANEL_BACK_LEFT) {
            extractor->extractBackLeftPanelsImage();
        }
        else if (type == PANEL_BACK_RIGHT) {
            extractor->extractBackRightPanelsImage();
        }
        else if (type == PANEL_FORE_LEFT) {
            extractor->extractForeLeftPanelsImage();
        }
        else if (type == PANEL_FORE_RIGHT) {
            extractor->extractForeRightPanelsImage();
        }
        else if (type == BACK_PAR) {
            extractor->extractBacklightDatas();
            extractor->extractPARDatas();
        }
    }

    running = false;
}