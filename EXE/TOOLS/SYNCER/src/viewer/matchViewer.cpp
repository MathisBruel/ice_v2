#include "viewer/matchViewer.h"

MatchViewer::MatchViewer(Synchro::Match2 match, Synchro* synchro, QWidget *parent)
    : QWidget(parent)
{
    Synchronizable* synchronizable = synchro->getSynchronizable();
    CplFile* newCpl = synchro->getNewCpl();

    // -- calculate FRAMES
    double speedRateRef = synchronizable->getSpeedRate();
    if (speedRateRef == -1.0) {
        speedRateRef = synchronizable->getReels().at(0).speedRate;
    }
    double speedRateNew = newCpl->getReels().at(0).speedRate;

    // -- init widgets
    startFrameMatchRefLabel = new QLabel("Start timecode REF");
    startFrameMatchRef = new QLineEdit(QString::fromStdString(computeTimecode(match.startMsMatchRef, speedRateRef)));
    startFrameMatchRef->setReadOnly(true);
    startFrameMatchNewLabel = new QLabel("Start timecode NEW");
    startFrameMatchNew = new QLineEdit(QString::fromStdString(computeTimecode(match.startMsMatchNew, speedRateNew)));
    startFrameMatchNew->setReadOnly(true);

    endFrameMatchRefLabel = new QLabel("End timecode REF");
    endFrameMatchRef = new QLineEdit(QString::fromStdString(computeTimecode(match.endMsMatchRef, speedRateRef)));
    endFrameMatchRef->setReadOnly(true);
    endFrameMatchNewLabel = new QLabel("End timecode NEW");
    endFrameMatchNew = new QLineEdit(QString::fromStdString(computeTimecode(match.endMsMatchNew, speedRateNew)));
    endFrameMatchNew->setReadOnly(true);

    QHBoxLayout* line1 = new QHBoxLayout();
    line1->addWidget(startFrameMatchRefLabel);
    line1->addSpacing(5);
    line1->addWidget(startFrameMatchRef);
    line1->addSpacing(20);
    line1->addWidget(endFrameMatchRefLabel);
    line1->addSpacing(5);
    line1->addWidget(endFrameMatchRef);

    QHBoxLayout* line2 = new QHBoxLayout();
    line2->addWidget(startFrameMatchNewLabel);
    line2->addSpacing(5);
    line2->addWidget(startFrameMatchNew);
    line2->addSpacing(20);
    line2->addWidget(endFrameMatchNewLabel);
    line2->addSpacing(5);
    line2->addWidget(endFrameMatchNew);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addItem(line1);
    globalLayout->addItem(line2);
    setLayout(globalLayout);
}

MatchViewer::~MatchViewer()
{

}

std::string MatchViewer::computeTimecode(double ms, double speedRate)
{
    std::string timecode = "";
    int hour = ms / 3600000;
    ms -= hour * 3600000;
    int minute = ms / 60000;
    ms -= minute * 60000;
    int second = ms / 1000;
    ms -= second * 1000;
    int frame = (int)std::round(ms*speedRate);

    timecode += std::to_string(hour) + ":" + std::to_string(minute) + ":" + std::to_string(second) + ":" + std::to_string(frame);
    return timecode;
}