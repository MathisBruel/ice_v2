#include <QWidget>
#include <QSlider>
#include <QPixmap>
#include <QMessageBox>
#include <QBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QComboBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <algorithm>

#pragma once

class EditCommand : public QWidget
{
    Q_OBJECT

public:
    explicit EditCommand(QTableWidget* list, int currentRow, bool create, QWidget *parent = nullptr);
    ~EditCommand();

    void setData(QString typeStr, QString parameters);

private slots:

    void typeChanged(int index);
    void validating();
    void canceling();

private:

    QComboBox* type;
    QComboBox* projection;
    QComboBox* mode;
    QComboBox* force;

    QPushButton* validate;
    QPushButton* cancel;

    QTableWidget* list;
    int currentRow;
    bool create;
};
