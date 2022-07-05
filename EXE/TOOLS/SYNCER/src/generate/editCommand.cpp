#include "generate/editCommand.h"

EditCommand::EditCommand(QTableWidget* list, int currentRow, bool create, QWidget *parent)
    : QWidget(parent)
{
    this->list = list;
    this->create = create;
    this->currentRow = currentRow;

    type = new QComboBox();
    type->addItem("UNFORCE_ALL_DEVICES", 0);
    type->addItem("SWITCH_MODE", 1);
    type->addItem("LAUNCH_SCRIPT", 2);
    type->addItem("SWITCH_PROJECTION", 3);
    type->addItem("SET_PAR_AMBIANCE_ON", 4);
    type->addItem("SET_PAR_AMBIANCE_OFF", 5);
    connect(type, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)));

    projection = new QComboBox();
    projection->addItem("NO_FIT", 0);
    projection->addItem("FIT_X", 1);
    projection->addItem("FIT_Y", 2);
    projection->addItem("FIT_X_PROP_Y", 3);
    projection->addItem("FIT_Y_PROP_X", 4);
    projection->addItem("FIT_X_FIT_Y", 5);
    projection->hide();

    mode = new QComboBox();
    mode->addItem("OFF", 0);
    mode->addItem("MANUAL", 1);
    mode->addItem("SYNC", 2);
    mode->addItem("EXTERNAL", 3);
    mode->hide();

    force = new QComboBox();
    force->addItem("FALSE", 0);
    force->addItem("TRUE", 1);
    force->hide();

    validate = new QPushButton("Ok");
    connect(validate, SIGNAL(released()), this, SLOT(validating()));
    cancel = new QPushButton("Cancel");
    connect(cancel, SIGNAL(released()), this, SLOT(canceling()));

    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->addWidget(validate);
    controlLayout->addSpacing(5);
    controlLayout->addWidget(cancel);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addWidget(type);
    globalLayout->addWidget(projection);
    globalLayout->addWidget(mode);
    globalLayout->addWidget(force);
    globalLayout->addSpacing(20);
    globalLayout->addItem(controlLayout);
    setLayout(globalLayout);
}

EditCommand::~EditCommand()
{

}

void EditCommand::setData(QString typeStr, QString parameters)
{
    if (typeStr == "UNFORCE_ALL_DEVICES") {
        type->setCurrentIndex(0);
        typeChanged(0);
    }
    else if (typeStr == "SWITCH_MODE") {
        type->setCurrentIndex(1);
        typeChanged(1);

        if (parameters == "OFF") {
            mode->setCurrentIndex(0);
        }
        else if (parameters == "MANUAL") {
            mode->setCurrentIndex(1);
        }
        else if (parameters == "SYNC") {
            mode->setCurrentIndex(2);
        }
        else if (parameters == "EXTERNAL") {
            mode->setCurrentIndex(3);
        }
    }
    else if (typeStr == "LAUNCH_SCRIPT") {
        type->setCurrentIndex(2);
        typeChanged(2);

        if (parameters == "FALSE") {
            force->setCurrentIndex(0);
        }
        else if (parameters == "TRUE") {
            force->setCurrentIndex(1);
        }
    }
    else if (typeStr == "SWITCH_PROJECTION") {
        type->setCurrentIndex(3);
        typeChanged(3);

        if (parameters == "NO_FIT") {
            projection->setCurrentIndex(0);
        }
        else if (parameters == "FIT_X") {
            projection->setCurrentIndex(1);
        }
        else if (parameters == "FIT_Y") {
            projection->setCurrentIndex(2);
        }
        else if (parameters == "FIT_X_PROP_Y") {
            projection->setCurrentIndex(3);
        }
        else if (parameters == "FIT_Y_PROP_X") {
            projection->setCurrentIndex(4);
        }
        else if (parameters == "FIT_X_FIT_Y") {
            projection->setCurrentIndex(5);
        }
    }
    else if (typeStr == "SET_PAR_AMBIANCE_ON") {
        type->setCurrentIndex(4);
        typeChanged(4);
    }
    else if (typeStr == "SET_PAR_AMBIANCE_OFF") {
        type->setCurrentIndex(5);
        typeChanged(5);
    }
}

void EditCommand::typeChanged(int index)
{
    if (type->currentData().toInt() == 0 || type->currentData().toInt() == 4 || type->currentData().toInt() == 5) {
        projection->hide();
        mode->hide();
        force->hide();
    }
    else if (type->currentData().toInt() == 1) {
        projection->hide();
        mode->show();
        force->hide();
    }
    else if (type->currentData().toInt() == 2) {
        projection->hide();
        mode->hide();
        force->show();
    }
    else if (type->currentData().toInt() == 3) {
        projection->show();
        mode->hide();
        force->hide();
    }
}

void EditCommand::validating()
{
    if (create) {
        list->insertRow(currentRow);
    }
    list->setItem(currentRow, 0, new QTableWidgetItem(type->currentText()));

    if (type->currentData() == 1) {
        list->setItem(currentRow, 1, new QTableWidgetItem(mode->currentText()));
    }
    else if (type->currentData() == 2) {
        list->setItem(currentRow, 1, new QTableWidgetItem(force->currentText()));
    }
    else if (type->currentData() == 3) {
        list->setItem(currentRow, 1, new QTableWidgetItem(projection->currentText()));
    }
    else {
        list->setItem(currentRow, 1, new QTableWidgetItem(""));
    }

    this->close();
}
void EditCommand::canceling()
{
    this->close();
}