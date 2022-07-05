#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>
#include <QTableWidget>
#include <QFileDialog>
#include <QHeaderView>
#include <QTabWidget>
#include <QComboBox>
#include <QMessageBox>
#include <QCheckBox>
#include <QDebug>
#include <QTextEdit>
#include <iostream>
#include <fstream>

#include "syncFile.h"
#include "cplFile.h"
#include "synchronizable.h"
#include "synchro.h"
#include "viewer/synchroViewer.h"
#include "generate/cutSceneList.h"
#include "generate/commandList.h"

#pragma once

class SynchroWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SynchroWidget(QWidget *parent = nullptr);
    ~SynchroWidget();

private slots:

    void addSyncFile();
    void addCplFile();
    void setOutputDir();
    void launchSync();
    void generateSubtiles();

private:

    QPushButton* addSync;
    QPushButton* addCpl;
    QPushButton* outputSync;
    QPushButton* synchronize;
    QPushButton* genSubtiles;
    QComboBox* modeSync;
    QLineEdit* nameOfSync;
    QLineEdit* nameOfCpl;

    QTabWidget* options;
    CutSceneList* cutsList;
    CommandList* startCommandList;
    CommandList* endCommandList;

    CutScenes* cuts;
    CplFile* cplRef;
    Synchronizable* inSync;
    SynchroViewer* viewer;
    Synchro* synchroEngine;

    QString folder;
};
