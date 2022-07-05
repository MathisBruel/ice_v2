#include <QWidget>
#include <QMessageBox>
#include <QBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>

#include "cplFile.h"

#pragma once

class ReelViewer : public QWidget
{
    Q_OBJECT

public:
    explicit ReelViewer(CplFile::Reel reel, QWidget *parent = nullptr);
    ~ReelViewer();

private:

    QLabel* idLabel;
    QLineEdit* id;
    QLabel* speedRateLabel;
    QLineEdit* speedRate;
    QLabel* durationLabel;
    QLineEdit* duration;
    QLabel* keyLabel;
    QLineEdit* key;
};
