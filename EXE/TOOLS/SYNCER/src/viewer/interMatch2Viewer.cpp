#include "viewer/interMatch2Viewer.h"

InterMatch2Viewer::InterMatch2Viewer(Synchro::InterMatch inter, Synchro* synchro, QWidget *parent)
    : QWidget(parent)
{
    Synchronizable* synchronizable = synchro->getSynchronizable();
    CplFile* newCpl = synchro->getNewCpl();

    QVBoxLayout* globalLayout = new QVBoxLayout();

    // -- calculate FRAMES
    double speedRateRef = synchronizable->getSpeedRate();
    if (speedRateRef == -1.0) {
        speedRateRef = synchronizable->getReels().at(0).speedRate;
    }
    double speedRateNew = newCpl->getReels().at(0).speedRate;

    if (inter.startMsMatchNew == -1 && inter.endMsMatchNew == -1) {

        // -- entry point
        if (inter.startMsMatchRef == 0) {
            startFrameMatchRefLabel = new QLabel("Entry point");
            startFrameMatchRef = new QLineEdit(QString::fromStdString(computeTimecode(inter.endMsMatchRef, speedRateRef)));
            startFrameMatchRef->setReadOnly(true);
            
        }

        // -- start delay
        else if (inter.endMsMatchRef == 0){
            startFrameMatchRefLabel = new QLabel("Start delay");
            startFrameMatchRef = new QLineEdit(QString::fromStdString(computeTimecode(inter.startMsMatchRef, speedRateRef)));
            startFrameMatchRef->setReadOnly(true);
        }

        else {
            return;
        }

        QHBoxLayout* line1 = new QHBoxLayout();
        line1->addWidget(startFrameMatchRefLabel);
        line1->addSpacing(5);
        line1->addWidget(startFrameMatchRef);
        globalLayout->addItem(line1);

    }

    // -- Too much Reel do not match to match at start
    else if (inter.startMsMatchNew == -1) {
        startFrameMatchRef = new QLineEdit("Too much reel do not match at the start : " + QString::number(inter.endMsMatchNew));
        startFrameMatchRef->setReadOnly(true);

        QHBoxLayout* line1 = new QHBoxLayout();
        line1->addWidget(startFrameMatchRef);
        globalLayout->addItem(line1);
    }
    // -- Too much Reel do not match to match at end
    else if (inter.endMsMatchNew == -1) {
        startFrameMatchRef = new QLineEdit("Too much reel do not match at the end : " + QString::number(inter.startMsMatchNew));
        startFrameMatchRef->setReadOnly(true);

        QHBoxLayout* line1 = new QHBoxLayout();
        line1->addWidget(startFrameMatchRef);
        globalLayout->addItem(line1);
    }
    else {

        // -- init widgets
        startFrameMatchRefLabel = new QLabel("Start REF");
        
        startFrameMatchRef = new QLineEdit(QString::fromStdString(computeTimecode(inter.startMsMatchRef, speedRateRef)));
        startFrameMatchRef->setReadOnly(true);
        startFrameMatchNewLabel = new QLabel("Start NEW");
        startFrameMatchNew = new QLineEdit(QString::fromStdString(computeTimecode(inter.startMsMatchNew, speedRateNew)));
        startFrameMatchNew->setReadOnly(true);

        endFrameMatchRefLabel = new QLabel("End REF");
        endFrameMatchRef = new QLineEdit(QString::fromStdString(computeTimecode(inter.endMsMatchRef, speedRateRef)));
        endFrameMatchRef->setReadOnly(true);
        endFrameMatchNewLabel = new QLabel("End NEW");
        endFrameMatchNew = new QLineEdit(QString::fromStdString(computeTimecode(inter.endMsMatchNew, speedRateNew)));
        endFrameMatchNew->setReadOnly(true);

        int startFrameRef = inter.startMsMatchRef * speedRateRef / 1000.0 + 0.5;
        int endFrameRef = inter.endMsMatchRef * speedRateRef / 1000.0 + 0.5;
        int startFrameNew = inter.startMsMatchNew * speedRateNew / 1000.0 + 0.5;
        int endFrameNew = inter.endMsMatchNew * speedRateNew / 1000.0 + 0.5;

        QLabel* diffRefLabel = new QLabel("Diff REF");
        diffRef = new QLineEdit(QString::number(endFrameRef-startFrameRef-1));
        diffRef->setReadOnly(true);

        QLabel* diffNewLabel = new QLabel("Diff NEW");
        diffNew = new QLineEdit(QString::number(endFrameNew-startFrameNew-1));
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

        globalLayout->addItem(line1);
        globalLayout->addItem(line2);
        globalLayout->addItem(line3);
    }

    setLayout(globalLayout);
}

InterMatch2Viewer::~InterMatch2Viewer()
{

}

std::string InterMatch2Viewer::computeTimecode(double ms, double speedRate)
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