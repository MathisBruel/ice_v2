#include "viewer/syncViewer.h"

SyncViewer::SyncViewer(QWidget *parent)
    : QWidget(parent)
{
    this->cis = nullptr;
    this->cpl = nullptr;
    this->cuts = nullptr;
    loop = false;

    setMinimumHeight(100);
    setMinimumWidth(parent->width());

    startDelay = entryPoint = 0;
}
SyncViewer::~SyncViewer()
{

}

void SyncViewer::setData(Cis* cis, CplFile* cpl, CutScenes* cuts)
{
    this->cis = cis;
    this->cpl = cpl;
    this->cuts = cuts;
    repaint();
}

void SyncViewer::setTimings(int entryPoint, int startDelay)
{
    this->entryPoint = entryPoint;
    this->startDelay = startDelay;
    repaint();
}

void SyncViewer::paintEvent(QPaintEvent *event)
{
    if (cis != nullptr && cpl != nullptr) {
        int nbFrameCis = cis->getFrameCount();
        double speedRateValue = cpl->getReels().at(0).speedRate;
        double durationMs = cpl->getTotalMsDuration(nullptr);
        int nbFrameCpl = durationMs*speedRateValue/1000;
        int sizeCuts = 0;
        if (cuts != nullptr) {
            for (int i = 0; i < cuts->getListOfCuts().size(); i++) {
                sizeCuts += cuts->getListOfCuts().at(i).endFrame - cuts->getListOfCuts().at(i).startFrame;
            }
        }

        double sizeMax = (double)std::max(nbFrameCis+startDelay+entryPoint, nbFrameCpl+entryPoint);
        int sizeCis = ((double)nbFrameCis * (double)width() / sizeMax);
        int sizeCpl = ((double)nbFrameCpl * (double)width() / sizeMax);

        if (sizeCis <= 0) {sizeCis = 1;}

        int offsetCis = 0;
        int offsetCpl = 0;
        if (startDelay > 0) {
            offsetCis = ((double)startDelay * (double)width() / sizeMax);
        }
        else if (entryPoint > 0) {
            offsetCpl = ((double)entryPoint * (double)width() / sizeMax);
        }

        QPainter painter;
        painter.begin(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // -- paint CIS
        painter.fillRect(offsetCis, 5, sizeCis, 40, QColor(0, 0, 200));
        painter.drawRect(offsetCis, 5, sizeCis, 40);

        // -- paint cuts on cis
        if (cuts != nullptr) {
            for (int i = 0; i < cuts->getListOfCuts().size(); i++) {

                int offsetCut = offsetCpl + ((double)cuts->getListOfCuts().at(i).startFrame * (double)width() / sizeMax);
                int widthCut = ((double)(cuts->getListOfCuts().at(i).endFrame - cuts->getListOfCuts().at(i).startFrame) * (double)width() / sizeMax);

                if (offsetCut+widthCut < offsetCis) {
                    continue;
                }
                else if (offsetCut > offsetCis+sizeCis) {
                    continue;
                }

                painter.fillRect(std::max(offsetCut, offsetCis), 5, std::min(offsetCut+widthCut, offsetCis+sizeCis)-std::max(offsetCut, offsetCis), 40, QColor(200, 0, 0));
                painter.drawRect(std::max(offsetCut, offsetCis), 5, std::min(offsetCut+widthCut, offsetCis+sizeCis)-std::max(offsetCut, offsetCis), 40);
            }
        }

        // -- paint REEL
        int offsetReel = offsetCpl;
        for (int i = 0; i < cpl->getReels().size(); i++) {
            int sizeReel = ((double)cpl->getReels().at(i).duration * (double)width() / sizeMax);

            if (i < 180) {
                painter.fillRect(offsetReel, 50, sizeReel, 40, QColor(0, 255-i, 0));
            }
            else {
                painter.fillRect(offsetReel, 50, sizeReel, 40, QColor(0, 75, 0));
            }
            painter.drawRect(offsetReel, 50, sizeReel, 40);
            offsetReel += sizeReel;
        }

        // -- paint cuts on  reel
        if (cuts != nullptr) {
            for (int i = 0; i < cuts->getListOfCuts().size(); i++) {
                int offsetCut = offsetCpl + ((double)cuts->getListOfCuts().at(i).startFrame * (double)width() / sizeMax);
                int widthCut = ((double)(cuts->getListOfCuts().at(i).endFrame - cuts->getListOfCuts().at(i).startFrame) * (double)width() / sizeMax);
                painter.fillRect(offsetCut, 50, widthCut, 40, QColor(200, 200, 0));
                painter.drawRect(offsetCut, 50, widthCut, 40);
            }
        }

        painter.end();
    }
}

void SyncViewer::mouseReleaseEvent(QMouseEvent * event)
{
    int nbFrameCis = cis->getFrameCount();
    double speedRateValue = cpl->getReels().at(0).speedRate;
    double durationMs = cpl->getTotalMsDuration(nullptr);
    int nbFrameCpl = durationMs*speedRateValue/1000;
    double sizeMax = (double)std::max(nbFrameCis+startDelay+entryPoint, nbFrameCpl);
    int sizeCis = ((double)nbFrameCis * (double)width() / sizeMax);
    int sizeCpl = ((double)nbFrameCpl * (double)width() / sizeMax);

    if (sizeCis <= 0) {sizeCis = 1;}

    int offsetCis = 0;
    int offsetCpl = 0;
    if (startDelay > 0) {
        offsetCis = ((double)startDelay * (double)width() / sizeMax);
    }
    else if (entryPoint > 0) {
        offsetCpl = ((double)entryPoint * (double)width() / sizeMax);
    }

    // -- CPL click
    if (event->y() >= 50 && event->y() < 90) {
        if (cpl != nullptr) {
            int offsetReel = offsetCpl;
            for (int i = 0; i < cpl->getReels().size(); i++) {
                int sizeReel = ((double)cpl->getReels().at(i).duration * (double)width() / sizeMax);

                if (event->x() >= offsetReel && event->x() < offsetReel + sizeReel) {
                    ReelViewer* reelViewer = new ReelViewer(cpl->getReels().at(i));
                    reelViewer->show();
                    break;   
                }
                offsetReel += sizeReel;
            }
        }
    }

    // -- CIS click
    else if (event->y() >= 5 && event->y() < 45) {

        if (cis != nullptr) {
            if (event->x() >= offsetCis && event->x() < offsetCis + sizeCis) {
                CisViewer* viewer = new CisViewer(cis, loop);
                viewer->show();
            }
        }
    }
}