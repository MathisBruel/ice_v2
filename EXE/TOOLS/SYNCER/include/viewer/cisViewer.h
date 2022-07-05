#include <QWidget>
#include <QSlider>
#include <QPixmap>
#include <QMessageBox>
#include <QBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include <QPushButton>

#include "cis.h"

#pragma once

class CisViewer : public QWidget
{
    Q_OBJECT

public:
    explicit CisViewer(QString path, bool loop = false, QWidget *parent = nullptr);
    explicit CisViewer(Cis* cis, bool loop = false, QWidget *parent = nullptr);
    ~CisViewer();

private slots:

    void changeCurrentFrame(int value);
    void moveFrameLeft();
    void moveFrameRight();

private:

    QLabel* image;
    QPushButton* frameLeft;
    QPushButton* frameRight;
    QLineEdit* positionFrame;
    QSlider* sliderFrame;

    Cis* cis;
    bool loop;
};
