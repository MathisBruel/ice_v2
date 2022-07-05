#include <QWidget>
#include <QMessageBox>
#include <QBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>

#include "synchro.h"

#pragma once

class InvalidCutViewer : public QWidget
{
    Q_OBJECT

public:
    explicit InvalidCutViewer(Synchro::InvalidCut invalidCut, Synchro* synchro, QWidget *parent = nullptr);
    ~InvalidCutViewer();

private:

    QLabel* startFrameLabel;
    QLineEdit* startFrame;
    QLabel* endFrameLabel;
    QLineEdit* endFrame;

    QLabel* positionLabel;
    QLineEdit* position;
    QLabel* deltaLabel;
    QLineEdit* delta;
};
