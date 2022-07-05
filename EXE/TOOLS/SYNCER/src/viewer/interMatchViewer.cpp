#include "viewer/interMatchViewer.h"

InterMatchViewer::InterMatchViewer(Synchro::InterMatch inter, Synchro* synchro, QWidget *parent)
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
    startFrameMatchRef = new QLineEdit(QString::fromStdString(computeTimecode(inter.startMsMatchRef, speedRateRef)));
    startFrameMatchRef->setReadOnly(true);
    startFrameMatchNewLabel = new QLabel("Start timecode NEW");
    startFrameMatchNew = new QLineEdit(QString::fromStdString(computeTimecode(inter.startMsMatchNew, speedRateNew)));
    startFrameMatchNew->setReadOnly(true);

    endFrameMatchRefLabel = new QLabel("End timecode REF");
    endFrameMatchRef = new QLineEdit(QString::fromStdString(computeTimecode(inter.endMsMatchRef, speedRateRef)));
    endFrameMatchRef->setReadOnly(true);
    endFrameMatchNewLabel = new QLabel("End timecode NEW");
    endFrameMatchNew = new QLineEdit(QString::fromStdString(computeTimecode(inter.endMsMatchNew, speedRateNew)));
    endFrameMatchNew->setReadOnly(true);

    QLabel* diffRefLabel = new QLabel("Diff REF");
    diffRef = new QLineEdit(QString::fromStdString(computeTimecode(inter.endMsMatchRef-inter.startMsMatchRef, speedRateRef)));
    diffRef->setReadOnly(true);

    QLabel* diffNewLabel = new QLabel("Diff NEW");
    diffNew = new QLineEdit(QString::fromStdString(computeTimecode(inter.endMsMatchNew-inter.startMsMatchNew, speedRateNew)));
    diffNew->setReadOnly(true);

    if (!inter.valid) {
        diffNew->setStyleSheet("QLineEdit { background: rgb(200, 0, 0); color: white }");
        diffRef->setStyleSheet("QLineEdit { background: rgb(200, 0, 0); color: white }");
    }

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

    QHBoxLayout* line3 = new QHBoxLayout();
    line3->addWidget(diffRefLabel);
    line3->addSpacing(5);
    line3->addWidget(diffRef);
    line3->addSpacing(20);
    line3->addWidget(diffNewLabel);
    line3->addSpacing(5);
    line3->addWidget(diffNew);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addItem(line1);
    globalLayout->addItem(line2);
    globalLayout->addItem(line3);
    setLayout(globalLayout);
}

InterMatchViewer::~InterMatchViewer()
{

}

std::string InterMatchViewer::computeTimecode(double ms, double speedRate)
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