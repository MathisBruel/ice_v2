#include "generate/editCut.h"

EditCut::EditCut(CutScenes* cuts, QTableWidget* list, int currentRow, bool create, QWidget *parent)
    : QWidget(parent)
{
    this->cuts = cuts;
    this->list = list;
    this->create = create;
    this->currentRow = currentRow;

    startFrameLabel = new QLabel("Start Frame");
    startFrame = new QLineEdit("");
    endFrameLabel = new QLabel("End Frame");
    endFrame = new QLineEdit("");
    descrLabel = new QLabel("Description");
    descr = new QLineEdit("");

    validate = new QPushButton("Ok");
    connect(validate, SIGNAL(released()), this, SLOT(validating()));
    cancel = new QPushButton("Cancel");
    connect(cancel, SIGNAL(released()), this, SLOT(canceling()));

    QHBoxLayout* frameLayout = new QHBoxLayout();
    frameLayout->addWidget(startFrameLabel);
    frameLayout->addSpacing(5);
    frameLayout->addWidget(startFrame);
    frameLayout->addSpacing(20);
    frameLayout->addWidget(endFrameLabel);
    frameLayout->addSpacing(5);
    frameLayout->addWidget(endFrame);

    QHBoxLayout* descrLayout = new QHBoxLayout();
    descrLayout->addWidget(descrLabel);
    descrLayout->addSpacing(5);
    descrLayout->addWidget(descr);

    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->addWidget(validate);
    controlLayout->addSpacing(5);
    controlLayout->addWidget(cancel);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addItem(frameLayout);
    globalLayout->addItem(descrLayout);
    globalLayout->addSpacing(20);
    globalLayout->addItem(controlLayout);
    setLayout(globalLayout);
}

EditCut::~EditCut()
{

}

void EditCut::setData(int start, int end, std::string description)
{
    startFrame->setText(QString::number(start));
    endFrame->setText(QString::number(end));
    descr->setText(QString::fromStdString(description));
}

void EditCut::validating()
{
    int start = 0;
    int end = 0;
    try {
        start = std::stoi(startFrame->text().toUtf8().constData());
        end = std::stoi(endFrame->text().toUtf8().constData());
    }
    catch (std::exception &e) {
        QMessageBox errorWindow;
        errorWindow.setText("Error while parsing frame number");
        errorWindow.exec();
        return;
    }

    if (start > end) {
        QMessageBox errorWindow;
        errorWindow.setText("Error start frame must be before end frame");
        errorWindow.exec();
        return;
    }

    if (create) {
        list->insertRow(currentRow);
        cuts->addCut(start, end, descr->text().toUtf8().constData());
    }
    else
    {
        cuts->replaceCut(start, end, descr->text().toUtf8().constData(), currentRow);
    }
    list->setItem(currentRow, 0, new QTableWidgetItem(startFrame->text()));
    list->setItem(currentRow, 1, new QTableWidgetItem(endFrame->text()));
    list->setItem(currentRow, 2, new QTableWidgetItem(descr->text()));

    emit validateCuts();

    this->close();
}
void EditCut::canceling()
{
    this->close();
}