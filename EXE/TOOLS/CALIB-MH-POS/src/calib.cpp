#include "calib.h"

CALIB::CALIB(QWidget *parent) :
    QWidget(parent)
{
    Poco::Logger::root().setChannel(new Poco::ConsoleChannel());
    Poco::Logger::root().setLevel(Poco::Message::PRIO_DEBUG);

    labelDevice = new QLabel("Device name");
    device = new QLineEdit("MH-LF");
    connect(device, SIGNAL(editingFinished()), this, SLOT(deviceChanged()));
    labelIp = new QLabel("IP ICE");
    ipICE = new QLineEdit("");
    connectToICE = new QPushButton("Connect");
    connect(connectToICE, SIGNAL(released()), this, SLOT(connectICE()));
    labelZoom = new QLabel("Zoom : 128");
    zoom = new QSlider();
    zoom->setOrientation(Qt::Horizontal);
    zoom->setMinimum(0);
    zoom->setMaximum(255);
    zoom->setValue(128);
    connect(zoom, SIGNAL(valueChanged(int)), this, SLOT(zoomChanged()));

    labelPointer = new QLabel("Pointer");
    pointer = new QSlider();
    pointer->setOrientation(Qt::Horizontal);
    pointer->setMinimum(0);
    pointer->setMaximum(1);
    connect(pointer, SIGNAL(valueChanged(int)), this, SLOT(ratioChanged()));
    labelPosition = new QLabel("Position (in mm)");
    posX = new QSpinBox();
    posX->setRange(0, 50000);
    connect(posX, SIGNAL(valueChanged(int)), this, SLOT(pointerChanged()));
    posY = new QSpinBox();
    posY->setRange(0, 50000);
    connect(posY, SIGNAL(valueChanged(int)), this, SLOT(pointerChanged()));
    posZ = new QSpinBox();
    posZ->setRange(0, 50000);
    connect(posZ, SIGNAL(valueChanged(int)), this, SLOT(pointerChanged()));

    labelMin = new QLabel("Minimum pointer (in mm)");
    minX = new QSpinBox();
    minX->setRange(0, 50000);
    connect(minX, SIGNAL(valueChanged(int)), this, SLOT(minChanged()));
    minY = new QSpinBox();
    minY->setRange(0, 50000);
    connect(minY, SIGNAL(valueChanged(int)), this, SLOT(minChanged()));
    minZ = new QSpinBox();
    minZ->setRange(0, 50000);
    connect(minZ, SIGNAL(valueChanged(int)), this, SLOT(minChanged()));

    labelMax = new QLabel("Maximum pointer (in mm)");
    maxX = new QSpinBox();
    maxX->setRange(0, 50000);
    connect(maxX, SIGNAL(valueChanged(int)), this, SLOT(maxChanged()));
    maxY = new QSpinBox();
    maxY->setRange(0, 50000);
    connect(maxY, SIGNAL(valueChanged(int)), this, SLOT(maxChanged()));
    maxZ = new QSpinBox();
    maxZ->setRange(0, 50000);
    connect(maxZ, SIGNAL(valueChanged(int)), this, SLOT(maxChanged()));

    QHBoxLayout* config = new QHBoxLayout();
    config->setSpacing(5);
    config->addWidget(labelDevice);
    config->addWidget(device);
    config->addWidget(labelIp);
    config->addWidget(ipICE);
    config->addWidget(connectToICE);

    QHBoxLayout* zoomLayout = new QHBoxLayout();
    zoomLayout->setSpacing(5);
    zoomLayout->addWidget(labelZoom);
    zoomLayout->addWidget(zoom);

    QHBoxLayout* pointerLayout = new QHBoxLayout();
    pointerLayout->setSpacing(5);
    pointerLayout->addWidget(labelPointer);
    pointerLayout->addWidget(pointer);

    QHBoxLayout* positionLayout = new QHBoxLayout();
    positionLayout->setSpacing(5);
    positionLayout->addWidget(labelPosition);
    positionLayout->addWidget(posX);
    positionLayout->addWidget(posY);
    positionLayout->addWidget(posZ);

    QHBoxLayout* minLayout = new QHBoxLayout();
    minLayout->setSpacing(5);
    minLayout->addWidget(labelMin);
    minLayout->addWidget(minX);
    minLayout->addWidget(minY);
    minLayout->addWidget(minZ);

    QHBoxLayout* maxLayout = new QHBoxLayout();
    maxLayout->setSpacing(5);
    maxLayout->addWidget(labelMax);
    maxLayout->addWidget(maxX);
    maxLayout->addWidget(maxY);
    maxLayout->addWidget(maxZ);

    QVBoxLayout* global = new QVBoxLayout();
    global->addItem(config);
    global->addItem(zoomLayout);
    global->addItem(pointerLayout);
    global->addItem(positionLayout);
    global->addItem(minLayout);
    global->addItem(maxLayout);

    render = new Render();
    timer = new QTimer();
    timer->setTimerType(Qt::PreciseTimer);
    timer->setInterval(300);
    connect(timer, SIGNAL(timeout()), render, SLOT(run()));

    setLayout(global);
}

CALIB::~CALIB()
{
    
}

void CALIB::deviceChanged()
{
    std::string deviceStr(device->text().toUtf8().constData());

    if (timer->isActive()) {
        timer->stop();
        Poco::Thread::sleep(500);
        render->changeDevice(deviceStr);
        timer->start();
    }
    else {
        render->changeDevice(deviceStr);
    }
}

void CALIB::connectICE() 
{
    QString ip = ipICE->text();
    if (!ip.isEmpty()) {
        render->connectICE(ip.toStdString());
        timer->start();
    }
}

void CALIB::zoomChanged() 
{
    QString str = "Zoom : " + QString::number(zoom->value());
    labelZoom->setText(str);
    render->changeZoom(zoom->value());
}

void CALIB::ratioChanged() 
{
    render->changeRatio(pointer->value());
}
void CALIB::pointerChanged()
{
    render->changePointer(posX->value(), posY->value(), posZ->value());
}
void CALIB::minChanged()
{
    render->changeMin(minX->value(), minY->value(), minZ->value());
}
void CALIB::maxChanged()
{
    render->changeMax(maxX->value(), maxY->value(), maxZ->value());
}