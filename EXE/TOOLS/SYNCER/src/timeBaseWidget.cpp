#include "timeBaseWidget.h"

TimeBaseWidget::TimeBaseWidget(QWidget *parent)
    : QWidget(parent)
{
    cplRef = nullptr;

    addCpl = new QPushButton(QIcon("images/add.png"), "");
    addCpl->setIconSize(QSize(50, 50));
    connect(addCpl, SIGNAL(released()), this, SLOT(addCplFile()));

    setReel = new QPushButton(QIcon("images/right-arrow.png"), "");
    setReel->setIconSize(QSize(50, 50));
    connect(setReel, SIGNAL(released()), this, SLOT(globalToReel()));

    setGlobal = new QPushButton(QIcon("images/left-arrow.png"), "");
    setGlobal->setIconSize(QSize(50, 50));
    connect(setGlobal, SIGNAL(released()), this, SLOT(reelToGlobal()));

    globalLabel = new QLabel("Global frame in reel");
    frameGlobal = new QLineEdit("0");
    reelLabel = new QLabel("Frame in reel");
    frameInReel = new QLineEdit("0");
    reelId = new QComboBox();

    QVBoxLayout* global = new QVBoxLayout();
    global->addWidget(globalLabel);
    global->addSpacing(5);
    global->addWidget(frameGlobal);

    QVBoxLayout* reel = new QVBoxLayout();
    reel->addWidget(reelLabel);
    reel->addSpacing(5);
    reel->addWidget(frameInReel);
    reel->addWidget(reelId);

    QHBoxLayout* formatLayout = new QHBoxLayout();
    formatLayout->addItem(global);
    formatLayout->addWidget(setReel);
    formatLayout->addSpacing(20);
    formatLayout->addWidget(setGlobal);
    formatLayout->addItem(reel);
    formatLayout->addStretch();

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addWidget(addCpl);
    globalLayout->addSpacing(30);
    globalLayout->addItem(formatLayout);
    globalLayout->addStretch();
    setLayout(globalLayout);
}

TimeBaseWidget::~TimeBaseWidget()
{

}

void TimeBaseWidget::addCplFile()
{
    QString file = QFileDialog::getOpenFileName(this, "Choose CPL file", QString(), "*.xml");
    if (!file.isEmpty()) {
        CplFile* cplNew = new CplFile(file.toUtf8().constData());
        
        if (!cplNew->load()) {
            QMessageBox errorWindow;
            errorWindow.setText("Error while loading CPL : " + file);
            errorWindow.exec();
        }
        else {

            if (cplRef != nullptr) {
                delete cplRef;
                cplRef = nullptr;
            }

            cplRef = cplNew;

            frameGlobal->setText("0");
            frameInReel->setText("0");
            reelId->clear();

            for (int i = 0; i < cplRef->getReels().size(); i++) {
                reelId->addItem("Reel " + QString::number(i+1), i);
            }
        }
    }
}

void TimeBaseWidget::globalToReel()
{
    if (cplRef == nullptr) {
        QMessageBox errorWindow;
        errorWindow.setText("Cpl not loaded !");
        errorWindow.exec();
        return;
    }

    int globalFrameInput = 0;
    try {
        std::string data = frameGlobal->text().toUtf8().constData();
        globalFrameInput = std::stoi(data);
    }
    catch (std::exception &e) {
        QMessageBox errorWindow;
        errorWindow.setText("Input is not a valid number");
        errorWindow.exec();
        return;
    }

    int offsetReel = 0;
    bool found = false;
    for (int i = 0; i < cplRef->getReels().size(); i++) {
        
        if (globalFrameInput >= offsetReel && globalFrameInput < offsetReel + cplRef->getReels().at(i).duration) {

            int relativeFrame = globalFrameInput - offsetReel;
            reelId->setCurrentIndex(i);
            frameInReel->setText(QString::number(relativeFrame));
            found = true;
            break;
        }
            
        offsetReel += cplRef->getReels().at(i).duration;
    }

    if (!found) {
        QMessageBox errorWindow;
        errorWindow.setText("Number is over size of CPL !");
        errorWindow.exec();
    }
}

void TimeBaseWidget::reelToGlobal()
{
    if (cplRef == nullptr) {
        QMessageBox errorWindow;
        errorWindow.setText("Cpl not loaded !");
        errorWindow.exec();
        return;
    }

    int relativeFrameInput = 0;
    try {
        std::string data = frameInReel->text().toUtf8().constData();
        relativeFrameInput = std::stoi(data);
    }
    catch (std::exception &e) {
        QMessageBox errorWindow;
        errorWindow.setText("Input is not a valid number");
        errorWindow.exec();
        return;
    }

    int offsetReel = 0;
    bool found = false;
    for (int i = 0; i < cplRef->getReels().size(); i++) {
        
        if (reelId->currentIndex() == i) {

            if (relativeFrameInput >= cplRef->getReels().at(i).duration) {
                break;
            }

            int globalFrame = offsetReel + relativeFrameInput;
            frameGlobal->setText(QString::number(globalFrame));
            found = true;
            break;
        }
            
        offsetReel += cplRef->getReels().at(i).duration;
    }

    if (!found) {
        QMessageBox errorWindow;
        errorWindow.setText("Frame specified is over size of reel");
        errorWindow.exec();
    }
}