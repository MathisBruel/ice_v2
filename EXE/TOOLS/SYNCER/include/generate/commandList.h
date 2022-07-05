#include <QWidget>
#include <QMessageBox>
#include <QBoxLayout>
#include <QDebug>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>

#pragma once

#include "generate/editCommand.h"

class CommandList : public QWidget
{
    Q_OBJECT

public:
    explicit CommandList(QWidget *parent = nullptr);
    ~CommandList();

    QTableWidget* getTable() {return listCommand;}

private slots:

    void addingCommand();
    void removingCommand();
    void viewCommand(int, int);

private:

    QPushButton* addCommand;
    QPushButton* removeCommand;
    QTableWidget* listCommand;
};
