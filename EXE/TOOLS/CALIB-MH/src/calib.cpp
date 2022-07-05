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

    pan = new QSlider();
    pan->setTickInterval(1);
    pan->setMinimum(-1080);
    pan->setMaximum(1080);
    pan->setSliderPosition(0);
    pan->setOrientation(Qt::Horizontal);
    connect(pan, SIGNAL(valueChanged(int)), this, SLOT(panChanged()));

    tilt = new QSlider();
    tilt->setTickInterval(1);
    tilt->setMinimum(-440);
    tilt->setMaximum(440);
    tilt->setSliderPosition(0);
    tilt->setOrientation(Qt::Horizontal);
    connect(tilt, SIGNAL(valueChanged(int)), this, SLOT(tiltChanged()));
    panEdit = new QLineEdit("Pan : 0°");
    tiltEdit = new QLineEdit("Tilt : 0°");

    QHBoxLayout* config = new QHBoxLayout();
    config->setSpacing(5);
    config->addWidget(labelDevice);
    config->addWidget(device);
    config->addWidget(labelIp);
    config->addWidget(ipICE);
    config->addWidget(connectToICE);

    QHBoxLayout* panLayout = new QHBoxLayout();
    panLayout->setSpacing(5);
    panLayout->addWidget(panEdit);
    panLayout->addWidget(pan);

    QHBoxLayout* tiltLayout = new QHBoxLayout();
    tiltLayout->setSpacing(5);
    tiltLayout->addWidget(tiltEdit);
    tiltLayout->addWidget(tilt);

    QVBoxLayout* global = new QVBoxLayout();
    global->addItem(config);
    global->addItem(panLayout);
    global->addItem(tiltLayout);

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

void CALIB::panChanged()
{
    double value = (double)pan->sliderPosition() / 4.0;
    QString str = "Pan : " + QString::number(value) + "°";
    panEdit->setText(str);
    render->changePan(value);
}
void CALIB::tiltChanged()
{
    double value = (double)tilt->sliderPosition() / 4.0;
    QString str = "Tilt : " + QString::number(value) + "°";
    tiltEdit->setText(str);
    render->changeTilt(value);
}