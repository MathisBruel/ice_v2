#include <QMainWindow>
#include <QTabWidget>
#include <QPushButton>
#include <QWidget>
#include <QBoxLayout>
#include <QTimer>
#include <QColor>
#include <QSlider>
#include <QLineEdit>
#include <QFileDialog>
#include <QCheckBox>

#include "Poco/Logger.h"
#include "Poco/ConsoleChannel.h"
#include "render.h"
#include "graph.h"
#include "labCalib.h"

#pragma once

class CALIB : public QWidget
{
    Q_OBJECT

public:
    explicit CALIB(QWidget *parent = nullptr);
    ~CALIB();

private slots:

    void loadLAB();
    void saveLAB();

    void changeFactorA(int value);
    void changeBiasA(int value);
    void changeFactorB(int value);
    void changeBiasB(int value);

private:

    QPushButton* load;
    QPushButton* save;

    QLineEdit* factorAEdit;
    QSlider* factorA;
    QLineEdit* biasAEdit;
    QSlider* biasA;
    QLineEdit* factorBEdit;
    QSlider* factorB;
    QLineEdit* biasBEdit;
    QSlider* biasB;

    // -- for rendering
    Render* render;
    QTimer* timer;

    // -- for calibrating 
    LabCalib* calib;
    Graph* lumGraph;
};
