#include "viewer/cisViewer.h"

CisViewer::CisViewer(QString path, bool loop, QWidget *parent)
    : QWidget(parent)
{
    this->loop = loop;

    // -- we open the cis file
    cis = new Cis();
    if (!cis->open(path.toUtf8().constData())) {
        QMessageBox errorWindow;
        errorWindow.setText("Error while opening : " + path);
        errorWindow.exec();
        close();
    }

    // -- we save curretn image locally for Qt to open later
    Image* frame = cis->getRGBAImage(0);
    frame->saveImage("tempFrame.png");
    delete frame;
    
    // -- init widgets
    image = new QLabel(path);
    QPixmap pic("tempFrame.png");
    image->setPixmap(pic);
    image->setStyleSheet("QLabel { background-color : black}");

    // -- control
    positionFrame = new QLineEdit("0");
    positionFrame->setReadOnly(true);
    positionFrame->setAlignment(Qt::AlignHCenter);
    frameLeft = new QPushButton(QIcon("images/left.png"), "");
    frameLeft->setIconSize(QSize(50, 50));
    connect(frameLeft, SIGNAL(released()), this, SLOT(moveFrameLeft()));
    frameRight = new QPushButton(QIcon("images/right.png"), "");
    frameRight->setIconSize(QSize(50, 50));
    connect(frameRight, SIGNAL(released()), this, SLOT(moveFrameRight()));

    // -- slider
    sliderFrame = new QSlider(Qt::Horizontal);
    sliderFrame->setMinimum(0);
    sliderFrame->setMaximum(cis->getFrameCount()-1);
    if (loop) {
        sliderFrame->setMaximum(cis->getFrameCount()*2-1);
    }
    sliderFrame->setSliderPosition(0);
    connect(sliderFrame, SIGNAL(valueChanged(int)), this, SLOT(changeCurrentFrame(int)));

    // -- set layouts
    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->addWidget(frameLeft);
    controlLayout->addSpacing(2);
    controlLayout->addWidget(positionFrame);
    controlLayout->addSpacing(2);
    controlLayout->addWidget(frameRight);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addWidget(image);
    globalLayout->addSpacing(2);
    globalLayout->addItem(controlLayout);
    globalLayout->addWidget(sliderFrame);
    setLayout(globalLayout);
}

CisViewer::CisViewer(Cis* cis, bool loop, QWidget *parent)
    : QWidget(parent)
{

    // -- we open the cis file
    this->cis = cis;
    this->loop = loop;

    // -- we save curretn image locally for Qt to open later
    Image* frame = cis->getRGBAImage(0);
    frame->saveImage("tempFrame.png");
    delete frame;
    
    // -- init widgets
    image = new QLabel("");
    QPixmap pic("tempFrame.png");
    image->setPixmap(pic);
    image->setStyleSheet("QLabel { background-color : black}");

    // -- control
    positionFrame = new QLineEdit("0");
    positionFrame->setReadOnly(true);
    positionFrame->setAlignment(Qt::AlignHCenter);
    frameLeft = new QPushButton(QIcon("images/left.png"), "");
    frameLeft->setIconSize(QSize(50, 50));
    connect(frameLeft, SIGNAL(released()), this, SLOT(moveFrameLeft()));
    frameRight = new QPushButton(QIcon("images/right.png"), "");
    frameRight->setIconSize(QSize(50, 50));
    connect(frameRight, SIGNAL(released()), this, SLOT(moveFrameRight()));

    // -- slider
    sliderFrame = new QSlider(Qt::Horizontal);
    sliderFrame->setMinimum(0);
    sliderFrame->setMaximum(cis->getFrameCount()-1);
    if (loop) {
        sliderFrame->setMaximum(cis->getFrameCount()*2-1);
    }
    sliderFrame->setSliderPosition(0);
    connect(sliderFrame, SIGNAL(valueChanged(int)), this, SLOT(changeCurrentFrame(int)));

    // -- set layouts
    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->addWidget(frameLeft);
    controlLayout->addSpacing(2);
    controlLayout->addWidget(positionFrame);
    controlLayout->addSpacing(2);
    controlLayout->addWidget(frameRight);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addWidget(image);
    globalLayout->addSpacing(2);
    globalLayout->addItem(controlLayout);
    globalLayout->addWidget(sliderFrame);
    setLayout(globalLayout);
}

CisViewer::~CisViewer()
{
    //delete cis;
}

void CisViewer::changeCurrentFrame(int value)
{
    // -- change position frame
    positionFrame->setText(QString::number(value));

    if (loop) {
        value = value % cis->getFrameCount();
    }
    
    // -- save new image for current frame
    Image* frame = cis->getRGBAImage(value);
    frame->saveImage("tempFrame.png");
    delete frame;

    // -- replace image to be displayed
    QPixmap pic("tempFrame.png");
    image->setPixmap(pic);
}

void CisViewer::moveFrameLeft()
{
    int position = sliderFrame->sliderPosition();
    if (position > 0) {
        sliderFrame->setSliderPosition(position-1);
    }
}

void CisViewer::moveFrameRight()
{
    int position = sliderFrame->sliderPosition();
    if (position < sliderFrame->maximum()) {
        sliderFrame->setSliderPosition(position+1);
    }
}