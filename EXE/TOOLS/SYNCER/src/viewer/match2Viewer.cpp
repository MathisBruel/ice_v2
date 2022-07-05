#include "viewer/match2Viewer.h"

Match2Viewer::Match2Viewer(Synchro::Match match, QWidget *parent)
    : QWidget(parent)
{
    // -- init widgets
    startReelRefLabel = new QLabel("Start Reef idx REF");
    startReelRef = new QLineEdit(QString::number(match.idxReelRef));
    startReelRef->setReadOnly(true);
    startReelNewLabel = new QLabel("Start Reef idx NEW");
    startReelNew = new QLineEdit(QString::number(match.idxReelNew));
    startReelNew->setReadOnly(true);

    endReelRefLabel = new QLabel("End Reel idx REF");
    endReelRef = new QLineEdit(QString::number(match.idxReelRef+match.nbIdxReelRef-1));
    endReelRef->setReadOnly(true);
    endReelNewLabel = new QLabel("End Reel idx NEW");
    endReelNew = new QLineEdit(QString::number(match.nbIdxReelNew));
    endReelNew->setReadOnly(true);

    QHBoxLayout* line1 = new QHBoxLayout();
    line1->addWidget(startReelRefLabel);
    line1->addSpacing(5);
    line1->addWidget(startReelRef);
    line1->addSpacing(20);
    line1->addWidget(endReelRefLabel);
    line1->addSpacing(5);
    line1->addWidget(endReelRef);

    QHBoxLayout* line2 = new QHBoxLayout();
    line2->addWidget(startReelNewLabel);
    line2->addSpacing(5);
    line2->addWidget(startReelNew);
    line2->addSpacing(20);
    line2->addWidget(endReelNewLabel);
    line2->addSpacing(5);
    line2->addWidget(endReelNew);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addItem(line1);
    globalLayout->addItem(line2);
    setLayout(globalLayout);
}

Match2Viewer::~Match2Viewer()
{

}