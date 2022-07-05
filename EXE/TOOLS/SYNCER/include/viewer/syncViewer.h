#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

#include "cis.h"
#include "cplFile.h"
#include "cutscenes.h"

#include "viewer/cisViewer.h"
#include "viewer/reelViewer.h"

#pragma once

class SyncViewer : public QWidget
{
    Q_OBJECT

public:
    explicit SyncViewer(QWidget *parent = nullptr);
    ~SyncViewer();

    void setData(Cis* cis, CplFile* cpl, CutScenes* cuts);
    void setTimings(int entryPoint, int startDelay);
    void setLoop(bool loop) {this->loop = loop;}

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

private:

    Cis* cis;
    CplFile* cpl;
    CutScenes* cuts;
    bool loop;

    int startDelay;
    int entryPoint;
};
