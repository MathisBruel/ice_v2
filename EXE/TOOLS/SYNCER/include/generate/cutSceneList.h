#include <QWidget>
#include <QMessageBox>
#include <QBoxLayout>
#include <QDebug>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>

#pragma once

#include "cutscenes.h"
#include "generate/editCut.h"

class CutSceneList : public QWidget
{
    Q_OBJECT

public:
    explicit CutSceneList(CutScenes* cuts, QWidget *parent = nullptr);
    ~CutSceneList();

signals:

    void updateCuts();

private slots:

    void addingCut();
    void removingCut();
    void viewCut(int, int);

private:

    QPushButton* addCut;
    QPushButton* removeCut;
    QTableWidget* listCuts;

    CutScenes* cuts;
};
