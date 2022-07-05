#include "viewer/reelViewer.h"

ReelViewer::ReelViewer(CplFile::Reel reel, QWidget *parent)
    : QWidget(parent)
{
    idLabel = new QLabel("ID");
    id = new QLineEdit(QString::fromStdString(reel.id));
    id->setReadOnly(true);

    speedRateLabel = new QLabel("SPEEDRATE");
    speedRate = new QLineEdit(QString::number(reel.speedRate));
    speedRate->setReadOnly(true);

    durationLabel = new QLabel("DURATION");
    duration = new QLineEdit(QString::number(reel.duration));
    duration->setReadOnly(true);

    keyLabel = new QLabel("KEY");
    key = new QLineEdit(QString::fromStdString(reel.keyId));
    key->setReadOnly(true);

    QHBoxLayout* line1 = new QHBoxLayout();
    line1->addWidget(idLabel);
    line1->addSpacing(5);
    line1->addWidget(id);

    QHBoxLayout* line2 = new QHBoxLayout();
    line2->addWidget(speedRateLabel);
    line2->addSpacing(5);
    line2->addWidget(speedRate);
    line2->addSpacing(20);
    line2->addWidget(durationLabel);
    line2->addSpacing(5);
    line2->addWidget(duration);

    QHBoxLayout* line3 = new QHBoxLayout();
    line3->addWidget(keyLabel);
    line3->addSpacing(5);
    line3->addWidget(key);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addItem(line1);
    globalLayout->addItem(line2);
    globalLayout->addItem(line3);
    setLayout(globalLayout);
}

ReelViewer::~ReelViewer()
{

}
