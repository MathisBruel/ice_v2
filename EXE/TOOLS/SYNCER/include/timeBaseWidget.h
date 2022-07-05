#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>
#include <QFileDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>

#include "cplFile.h"
#pragma once

class TimeBaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimeBaseWidget(QWidget *parent = nullptr);
    ~TimeBaseWidget();

private slots:

    void addCplFile();
    void globalToReel();
    void reelToGlobal();

private:

    QPushButton* addCpl;
    QPushButton* setReel;
    QPushButton* setGlobal;

    QLabel* globalLabel;
    QLineEdit* frameGlobal;
    QLabel* reelLabel;
    QLineEdit* frameInReel;
    QComboBox* reelId;

    CplFile* cplRef;
};
