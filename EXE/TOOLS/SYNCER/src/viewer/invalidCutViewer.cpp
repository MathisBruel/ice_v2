#include "viewer/invalidCutViewer.h"

InvalidCutViewer::InvalidCutViewer(Synchro::InvalidCut invalidCut, Synchro* synchro, QWidget *parent)
    : QWidget(parent)
{
    Synchronizable* synchronizable = synchro->getSynchronizable();
    CplFile* newCpl = synchro->getNewCpl();

    // -- calculate FRAMES
    double speedRate = synchronizable->getSpeedRate();
    if (speedRate == -1.0) {
        speedRate = synchronizable->getReels().at(0).speedRate;
    }
    int startFrameInt = invalidCut.startMs * speedRate / 1000.0 + 0.5;
    int endFrameInt = invalidCut.endMs * speedRate / 1000.0 + 0.5;

    // -- init widgets
    startFrameLabel = new QLabel("StartFrame");
    startFrame = new QLineEdit(QString::number(startFrameInt));
    startFrame->setReadOnly(true);
    endFrameLabel = new QLabel("EndFrame");
    endFrame = new QLineEdit(QString::number(endFrameInt));
    endFrame->setReadOnly(true);

    QString positionStr;
    int refDeltaFrame = 0;
    if (invalidCut.left && invalidCut.right) {
        positionStr = "Missing frame";
        refDeltaFrame = invalidCut.nbMissingFrame;
    }
    else if (invalidCut.left) {
        positionStr = "Excess frame at left";
        refDeltaFrame = invalidCut.nbFrameOutLeft;
    }
    else if (invalidCut.right) {
        positionStr = "Excess frame at right";
        refDeltaFrame = invalidCut.nbFrameOutRight;
    }

    positionLabel = new QLabel("Position delta");
    position = new QLineEdit(positionStr);
    position->setReadOnly(true);
    deltaLabel = new QLabel("Delta");
    delta = new QLineEdit(QString::number(refDeltaFrame));
    delta->setReadOnly(true);

    QHBoxLayout* line1 = new QHBoxLayout();
    line1->addWidget(startFrameLabel);
    line1->addSpacing(5);
    line1->addWidget(startFrame);
    line1->addSpacing(20);
    line1->addWidget(endFrameLabel);
    line1->addSpacing(5);
    line1->addWidget(endFrame);

    QHBoxLayout* line2 = new QHBoxLayout();
    line2->addWidget(positionLabel);
    line2->addSpacing(5);
    line2->addWidget(position);
    line2->addSpacing(20);
    line2->addWidget(deltaLabel);
    line2->addSpacing(5);
    line2->addWidget(delta);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addItem(line1);
    globalLayout->addItem(line2);
    setLayout(globalLayout);
}

InvalidCutViewer::~InvalidCutViewer()
{

}