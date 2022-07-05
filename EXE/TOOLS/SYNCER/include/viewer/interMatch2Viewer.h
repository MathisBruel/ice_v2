#include <QWidget>
#include <QMessageBox>
#include <QBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>

#include "synchro.h"

#pragma once

class InterMatch2Viewer : public QWidget
{
    Q_OBJECT

public:
    explicit InterMatch2Viewer(Synchro::InterMatch inter, Synchro* synchro, QWidget *parent = nullptr);
    ~InterMatch2Viewer();

    std::string computeTimecode(double ms, double speedRate);

private:

    QLabel* startFrameMatchRefLabel;
    QLineEdit* startFrameMatchRef;
    QLabel* startFrameMatchNewLabel;
    QLineEdit* startFrameMatchNew;
    QLabel* endFrameMatchRefLabel;
    QLineEdit* endFrameMatchRef;
    QLabel* endFrameMatchNewLabel;
    QLineEdit* endFrameMatchNew;

    QLabel* diffRefLabel;
    QLineEdit* diffRef;
    QLabel* diffNewLabel;
    QLineEdit* diffNew;
};
