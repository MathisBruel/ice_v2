#include <QWidget>
#include <QSlider>
#include <QPixmap>
#include <QMessageBox>
#include <QBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QComboBox>
#include <QTabWidget>
#include <QGridLayout>
#include <QFileDialog>
#include <QCheckBox>
#include <QTableWidget>

#include "Poco/Path.h"

#pragma once

#include "Converter.h"
#include "cis.h"
#include "syncFile.h"
#include "lviFile.h"

#include "viewer/syncViewer.h"
#include "generate/cutSceneList.h"
#include "generate/commandList.h"

class GenerateSync : public QWidget
{
    Q_OBJECT

public:
    explicit GenerateSync(QWidget *parent = nullptr);
    ~GenerateSync();

private slots:

    void load();
    void loadCis();
    void output();
    void genSync();
    void versionChanged(int index);
    void loopChanged(int state);

    void updateViewer();

    void startDelayChanged(int value);
    void entryPointChanged(int value);

private:

    void showDataInfo(bool show);
    void showDataV2(bool show);

    QPushButton* loadCPL;
    QPushButton* loadCIS;
    QPushButton* outputPath;
    QPushButton* generate;
    QLineEdit* savePath;
    QLineEdit* cisPath;

    QLabel* idLabel;
    QLineEdit* id;
    QLabel* titleLabel;
    QLineEdit* title;
    QLabel* durationLabel;
    QLineEdit* duration;
    QLabel* speedRateLabel;
    QLineEdit* speedRate;
    QCheckBox* loop;
    
    QLabel* startDelayLabel;
    QLineEdit* startDelay;
    QSlider* startDelaySlider;
    QLabel* entryPointLabel;
    QLineEdit* entryPoint;
    QSlider* entryPointSlider;

    QComboBox* iceVersion;
    QComboBox* projection;

    // -- visualizer (show reels)
    SyncViewer* viewer;

    // -- contains cutscenes and automatic commands 
    QTabWidget* options;
    CutSceneList* cutsList;
    CommandList* startCommandList;
    CommandList* endCommandList;
    
    CplFile* cplRef;
    Cis* cis;

    QString folder;
    QString idCpl;
    std::string cisFile;
};
