#include <QWidget>
#include <QMessageBox>
#include <QBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>

#include "synchro.h"

#pragma once

class Match2Viewer : public QWidget
{
    Q_OBJECT

public:
    explicit Match2Viewer(Synchro::Match match, QWidget *parent = nullptr);
    ~Match2Viewer();

private:

    QLabel* startReelRefLabel;
    QLineEdit* startReelRef;
    QLabel* startReelNewLabel;
    QLineEdit* startReelNew;
    QLabel* endReelRefLabel;
    QLineEdit* endReelRef;
    QLabel* endReelNewLabel;
    QLineEdit* endReelNew;
};
