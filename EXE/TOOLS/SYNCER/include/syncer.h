#include <QMainWindow>
#include <QTabWidget>

#include "synchroWidget.h"
#include "Poco/Logger.h"
#include "Poco/ConsoleChannel.h"

#include "mergeTabWidget.h"
#include "generateSyncWidget.h"
#include "synchroWidget.h"
#include "timeBaseWidget.h"

#pragma once

class Syncer : public QMainWindow
{
    Q_OBJECT

public:
    explicit Syncer(QWidget *parent = nullptr);
    ~Syncer();

private:

    QTabWidget* tab;
    MergeWidget* merge;
    GenerateSync* gen;
    SynchroWidget* sync;
    TimeBaseWidget* time;
};
