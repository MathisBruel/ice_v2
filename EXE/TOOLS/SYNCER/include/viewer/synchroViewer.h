#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QPen>
#include <QStaticText>

#include "Poco/Random.h"

#include "synchro.h"
#include "cutscenes.h"
#include "viewer/reelViewer.h"
#include "viewer/matchViewer.h"
#include "viewer/match2Viewer.h"
#include "viewer/interMatchViewer.h"
#include "viewer/interMatch2Viewer.h"
#include "viewer/invalidCutViewer.h"

#pragma once

class SynchroViewer : public QWidget
{
    Q_OBJECT

public:
    explicit SynchroViewer(QWidget *parent = nullptr);
    ~SynchroViewer();

    void setData(Synchro* synchro, bool byId = true);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

private:

    std::map<std::string, int> reelColorRef;
    std::map<std::string, int> reelColorNew;

    Synchro* synchro;
    bool byId;
};
