#include <QWidget>
#include <QMessageBox>
#include <QBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>

#include "synchro.h"

#pragma once

class InterMatchViewer : public QWidget
{
    Q_OBJECT

public:
    explicit InterMatchViewer(Synchro::InterMatch inter, Synchro* synchro, QWidget *parent = nullptr);
    ~InterMatchViewer();

private:

    std::string computeTimecode(double ms, double speedRate);

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
