#include <QWidget>
#include <QMessageBox>
#include <QBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>

#include "synchro.h"

#pragma once

class MatchViewer : public QWidget
{
    Q_OBJECT

public:
    explicit MatchViewer(Synchro::Match2 match, Synchro* synchro, QWidget *parent = nullptr);
    ~MatchViewer();

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
};
