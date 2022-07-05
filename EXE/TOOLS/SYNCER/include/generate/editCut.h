#include <QWidget>
#include <QSlider>
#include <QPixmap>
#include <QMessageBox>
#include <QBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <algorithm>

#pragma once

#include "cutscenes.h"

class EditCut : public QWidget
{
    Q_OBJECT

public:
    explicit EditCut(CutScenes* cuts, QTableWidget* list, int currentRow, bool create, QWidget *parent = nullptr);
    ~EditCut();

    void setData(int start, int end, std::string description);

signals:

    void validateCuts();

private slots:

    void validating();
    void canceling();

private:

    QLabel* startFrameLabel;
    QLineEdit* startFrame;
    QLabel* endFrameLabel;
    QLineEdit* endFrame;
    QLabel* descrLabel;
    QLineEdit* descr;

    QPushButton* validate;
    QPushButton* cancel;

    CutScenes* cuts;
    QTableWidget* list;
    int currentRow;
    bool create;
};
