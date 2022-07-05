#include "viewer/synchroViewer.h"

SynchroViewer::SynchroViewer(QWidget *parent)
    : QWidget(parent)
{
    this->synchro = nullptr;

    setMinimumHeight(240);
    setMinimumWidth(parent->width());
}
SynchroViewer::~SynchroViewer()
{

}

void SynchroViewer::setData(Synchro* synchro, bool byId)
{
    this->byId = byId;
    this->synchro = synchro;
    reelColorNew.clear();
    reelColorRef.clear();
    repaint();
}

void SynchroViewer::paintEvent(QPaintEvent *event)
{
    if (synchro != nullptr) {

        Poco::Random rand;

        // -- get total nb of frame (cuts are counted) 
        Synchronizable* synchronizableRef = synchro->getSynchronizable();
        CplFile* newCpl = synchro->getNewCpl();
        CutScenes* cuts = synchro->getCuts();


        int durationRef = synchronizableRef->getTotalMsDuration(nullptr);
        int durationNew = newCpl->getTotalMsDuration(nullptr);
        double ratioWidth = ((double)width()*0.95) / (double)std::max(durationRef, durationNew);
        int globalOffset = ((double)width()*0.025);

        std::map<std::string, int>::iterator it;

        QPainter painter;
        painter.begin(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.drawStaticText(3, 0, QStaticText("Reels of new CPL :"));
        
        // -- paint REEL of new CPL
        int offsetReelNew = globalOffset;
        for (int i = 0; i < newCpl->getReels().size(); i++) {

            int colorComponent = rand.next(255)*255+rand.next(255);
            it = reelColorNew.find(newCpl->getReels().at(i).id);
            if (it != reelColorNew.end()) {
                colorComponent = it->second;
            }
            else {
                reelColorNew.insert_or_assign(newCpl->getReels().at(i).id, colorComponent);
            }
            int sizeReel = ((double)newCpl->getReels().at(i).duration * 1000.0 * ratioWidth / newCpl->getReels().at(i).speedRate);
                
            painter.fillRect(offsetReelNew, 15, sizeReel, 40, QColor(0, colorComponent/255, (colorComponent%255)));
            painter.drawRect(offsetReelNew, 15, sizeReel, 40);
            offsetReelNew += sizeReel;
        }
        painter.drawLine(0, 57, width(), 57);

        painter.drawStaticText(3, 60, QStaticText("Reels of REF synchronized content :"));

        // -- paint REEL of REF
        int offsetReel = globalOffset;
        double speedRate = synchronizableRef->getSpeedRate();
        for (int i = 0; i < synchronizableRef->getReels().size(); i++) {

            int colorComponent = rand.next(255)*255+rand.next(255);
            it = reelColorRef.find(synchronizableRef->getReels().at(i).id);
            if (it != reelColorRef.end()) {
                colorComponent = it->second;
            }
            else {
                reelColorRef.insert_or_assign(synchronizableRef->getReels().at(i).id, colorComponent);
            }

            if (speedRate == -1.0) {
                speedRate = synchronizableRef->getReels().at(i).speedRate;
            }
            int sizeReel = ((double)synchronizableRef->getReels().at(i).duration * 1000.0 * ratioWidth / speedRate);

            painter.fillRect(offsetReel, 75, sizeReel, 40, QColor(colorComponent/255, (colorComponent%255), 0));
            painter.drawRect(offsetReel, 75, sizeReel, 40);
            offsetReel += sizeReel;
        }

        // -- paint cuts on  reel
        if (cuts != nullptr) {
            for (int i = 0; i < cuts->getListOfCuts().size(); i++) {
                int offsetCut = globalOffset + ((double)cuts->getListOfCuts().at(i).startFrame * 1000.0 * ratioWidth / speedRate) + 0.5;
                int widthCut = ((double)(cuts->getListOfCuts().at(i).endFrame - cuts->getListOfCuts().at(i).startFrame) * 1000.0 * ratioWidth / speedRate) + 0.5;
                painter.fillRect(offsetCut, 75, widthCut, 40, QColor(200, 200, 0));
                painter.drawRect(offsetCut, 75, widthCut, 40);
            }
        }
        painter.drawLine(0, 117, width(), 117);

        if (byId) {
            // -- paint matches
            painter.drawStaticText(3, 120, QStaticText("Matches :"));
            std::vector<Synchro::Match2> matches = synchro->getMatch2();
            for (int i = 0; i < matches.size(); i++) {
                int offsetMatch = globalOffset + ((matches.at(i).startMsMatchRef * ratioWidth) + 0.5);
                int widthMatch = ((matches.at(i).endMsMatchRef - matches.at(i).startMsMatchRef) * ratioWidth) + 0.5;
                painter.fillRect(offsetMatch, 135, widthMatch, 40, QColor(200, 0, 200));
                painter.drawRect(offsetMatch, 135, widthMatch, 40);
            }

            painter.drawLine(0, 177, width(), 177);

            // -- paint intermatches (valid and not valid)
            painter.drawStaticText(3, 180, QStaticText("Fill :"));
            std::vector<Synchro::InterMatch> inters = synchro->getInter();
            for (int i = 0; i < inters.size(); i++) {
                int offsetInter = globalOffset + (inters.at(i).startMsMatchRef * ratioWidth) + 0.5;
                int widthInter = ((inters.at(i).endMsMatchRef - inters.at(i).startMsMatchRef) * ratioWidth) + 0.5;

                if (widthInter < 10) {
                    offsetInter = offsetInter + widthInter/2 - 5;
                    widthInter = 10;
                }

                if (inters.at(i).valid) {
                    painter.fillRect(offsetInter, 195, widthInter, 40, QColor(100, 0, 100));
                    painter.drawRect(offsetInter, 195, widthInter, 40);
                }
                else {
                    painter.fillRect(offsetInter, 195, widthInter, 40, QColor(200, 0, 0));
                    painter.drawRect(offsetInter, 195, widthInter, 40);
                }
            }

            // -- paint invalid cuts
            std::vector<Synchro::InvalidCut> invaliCuts = synchro->getInvalidCuts();
            for (int i = 0; i < invaliCuts.size(); i++) {
                int offsetInvalid = globalOffset + (invaliCuts.at(i).startMs * ratioWidth) + 0.5;
                int widthInvalid = ((invaliCuts.at(i).endMs - invaliCuts.at(i).startMs) * ratioWidth) + 0.5;

                if (widthInvalid < 10) {
                    offsetInvalid = offsetInvalid + widthInvalid/2 - 5;
                    widthInvalid = 10;
                }

                painter.fillRect(offsetInvalid, 195, widthInvalid, 40, QColor(255, 128, 0));
                painter.drawRect(offsetInvalid, 195, widthInvalid, 40);
            }
            painter.drawLine(0, 237, width(), 237);
        }

        else {
            // -- paint matches
            painter.drawStaticText(3, 120, QStaticText("Matches :"));
            std::vector<Synchro::Match> matches = synchro->getMatch();
            for (int i = 0; i < matches.size(); i++) {
                if (matches.at(i).idxReelNew != -1 && matches.at(i).nbIdxReelNew != -1){

                    int offsetMatch = globalOffset + synchronizableRef->getTotalMsDurationUntilIdx(matches.at(i).idxReelRef, nullptr) * ratioWidth + 0.5;
                    int widthMatch = synchronizableRef->getTotalMsDurationBetweenIdx(matches.at(i).idxReelRef, matches.at(i).idxReelRef+matches.at(i).nbIdxReelRef-1, nullptr) * ratioWidth + 0.5;
                    painter.fillRect(offsetMatch, 135, widthMatch, 40, QColor(200, 0, 200));
                    painter.drawRect(offsetMatch, 135, widthMatch, 40);
                }
            }

            painter.drawLine(0, 177, width(), 177);

            // -- paint intermatches (valid and not valid)
            painter.drawStaticText(3, 180, QStaticText("Fill :"));
            std::vector<Synchro::InterMatch> inters = synchro->getInter();
            for (int i = 0; i < inters.size(); i++) {
                int offsetInter = globalOffset + (inters.at(i).startMsMatchRefUncut * ratioWidth) + 0.5;
                int widthInter = ((inters.at(i).endMsMatchRefUncut - inters.at(i).startMsMatchRefUncut) * ratioWidth) + 0.5;

                if (widthInter < 10) {
                    offsetInter = offsetInter + widthInter/2 - 5;
                    widthInter = 10;
                }

                if (inters.at(i).valid) {
                    painter.fillRect(offsetInter, 195, widthInter, 40, QColor(100, 0, 100));
                    painter.drawRect(offsetInter, 195, widthInter, 40);
                }
                else {
                    painter.fillRect(offsetInter, 195, widthInter, 40, QColor(200, 0, 0));
                    painter.drawRect(offsetInter, 195, widthInter, 40);
                }
            }
        }

        painter.end();
    }
}

void SynchroViewer::mouseReleaseEvent(QMouseEvent * event)
{
    if (synchro != nullptr) {

        // -- get total nb of frame (cuts are counted) 
        Synchronizable* synchronizableRef = synchro->getSynchronizable();
        CplFile* newCpl = synchro->getNewCpl();
        CutScenes* cuts = synchro->getCuts();


        int durationRef = synchronizableRef->getTotalMsDuration(nullptr);
        int durationNew = newCpl->getTotalMsDuration(nullptr);
        double ratioWidth = ((double)width()*0.95) / (double)std::max(durationRef, durationNew);
        int globalOffset = ((double)width()*0.025);

        // -- new CPL
        if (event->y() >= 15 && event->y() < 55) {
            int offsetReelNew = globalOffset;
            for (int i = 0; i < newCpl->getReels().size(); i++) {
                int sizeReel = ((double)newCpl->getReels().at(i).duration * 1000.0 * ratioWidth / newCpl->getReels().at(i).speedRate);
                
                if (event->x() >= offsetReelNew && event->x() < offsetReelNew + sizeReel) {
                    ReelViewer* reelViewer = new ReelViewer(newCpl->getReels().at(i));
                    reelViewer->show();
                    break;
                }
                offsetReelNew += sizeReel;
            }
        }

        // -- reference CPL
        else if (event->y() >= 75 && event->y() < 115) {
            int offsetReel = globalOffset;
            double speedRate = synchronizableRef->getSpeedRate();
            for (int i = 0; i < synchronizableRef->getReels().size(); i++) {

                if (speedRate == -1.0) {
                    speedRate = synchronizableRef->getReels().at(i).speedRate;
                }
                int sizeReel = ((double)synchronizableRef->getReels().at(i).duration * 1000.0 * ratioWidth / speedRate);

                if (event->x() >= offsetReel && event->x() < offsetReel + sizeReel) {
                    ReelViewer* reelViewer = new ReelViewer(synchronizableRef->getReels().at(i));
                    reelViewer->show();
                    break;
                }
                offsetReel += sizeReel;
            }
        }

        // -- match
        else if (event->y() >= 135 && event->y() < 175) {

            if (byId) {
                std::vector<Synchro::Match2> matches = synchro->getMatch2();
                for (int i = 0; i < matches.size(); i++) {
                    int offsetMatch = globalOffset + ((matches.at(i).startMsMatchRef * ratioWidth) + 0.5);
                    int widthMatch = ((matches.at(i).endMsMatchRef - matches.at(i).startMsMatchRef) * ratioWidth) + 0.5;

                    if (event->x() >= offsetMatch && event->x() < offsetMatch + widthMatch) {
                        MatchViewer* matchViewer = new MatchViewer(matches.at(i), synchro);
                        matchViewer->show();
                        break;
                    }
                }
            }
            else {
                std::vector<Synchro::Match> matches = synchro->getMatch();
                for (int i = 0; i < matches.size(); i++) {
                    if (matches.at(i).idxReelNew != -1 && matches.at(i).nbIdxReelNew != -1){

                        int offsetMatch = globalOffset + synchronizableRef->getTotalMsDurationUntilIdx(matches.at(i).idxReelRef, nullptr) * ratioWidth + 0.5;
                        int widthMatch = synchronizableRef->getTotalMsDurationBetweenIdx(matches.at(i).idxReelRef, matches.at(i).idxReelRef+matches.at(i).nbIdxReelRef-1, nullptr) * ratioWidth + 0.5;
                        
                        if (event->x() >= offsetMatch && event->x() < offsetMatch + widthMatch) {
                            Match2Viewer* matchViewer = new Match2Viewer(matches.at(i));
                            matchViewer->show();
                            break;
                        }
                    }
                }   
            }
        }

        // -- inter matches
        else if (event->y() >= 195 && event->y() < 235) {

            if (byId) {
                std::vector<Synchro::InterMatch> inters = synchro->getInter();
                for (int i = 0; i < inters.size(); i++) {
                    int offsetInter = globalOffset + (inters.at(i).startMsMatchRef * ratioWidth) + 0.5;
                    int widthInter = ((inters.at(i).endMsMatchRef - inters.at(i).startMsMatchRef) * ratioWidth) + 0.5;

                    if (widthInter < 10) {
                        offsetInter = offsetInter + widthInter/2 - 5;
                        widthInter = 10;
                    }

                    if (event->x() >= offsetInter && event->x() < offsetInter + widthInter) {
                        InterMatchViewer* interMatchViewer = new InterMatchViewer(inters.at(i), synchro);
                        interMatchViewer->show();
                        break;
                    }
                }

                // -- paint invalid cuts
                std::vector<Synchro::InvalidCut> invaliCuts = synchro->getInvalidCuts();
                for (int i = 0; i < invaliCuts.size(); i++) {
                    int offsetInvalid = globalOffset + (invaliCuts.at(i).startMs * ratioWidth) + 0.5;
                    int widthInvalid = ((invaliCuts.at(i).endMs - invaliCuts.at(i).startMs) * ratioWidth) + 0.5;

                    if (widthInvalid < 10) {
                        offsetInvalid = offsetInvalid + widthInvalid/2 - 5;
                        widthInvalid = 10;
                    }

                    if (event->x() >= offsetInvalid && event->x() < offsetInvalid + widthInvalid) {
                        InvalidCutViewer* invalidCutViewer = new InvalidCutViewer(invaliCuts.at(i), synchro);
                        invalidCutViewer->show();
                        break;
                    }
                }
            }

            else {
                std::vector<Synchro::InterMatch> inters = synchro->getInter();
                for (int i = 0; i < inters.size(); i++) {
                    int offsetInter = globalOffset + (inters.at(i).startMsMatchRefUncut * ratioWidth) + 0.5;
                    int widthInter = ((inters.at(i).endMsMatchRefUncut - inters.at(i).startMsMatchRefUncut) * ratioWidth) + 0.5;

                    if (widthInter < 10) {
                        offsetInter = offsetInter + widthInter/2 - 5;
                        widthInter = 10;
                    }

                    if (event->x() >= offsetInter && event->x() < offsetInter + widthInter) {
                        InterMatch2Viewer* interMatchViewer = new InterMatch2Viewer(inters.at(i), synchro);
                        interMatchViewer->show();
                        break;
                    }
                }
            }
        }
    }
}