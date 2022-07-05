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

    timeEdit = new QLineEdit("500 ms");
    timeSlider = new QSlider(Qt::Horizontal);
    timeSlider->setTickInterval(100);
    timeSlider->setRange(500, 5000);
    timeSlider->setValue(500);
    connect(timeSlider, SIGNAL(valueChanged(int)), this, SLOT(frequencyChanged()));

    redEdit = new QLineEdit("0 red");
    redSlider = new QSlider(Qt::Horizontal);
    redSlider->setTickInterval(1);
    redSlider->setRange(0, 255);
    redSlider->setValue(0);
    connect(redSlider, SIGNAL(valueChanged(int)), this, SLOT(redChanged()));

    greenEdit = new QLineEdit("0 green");
    greenSlider = new QSlider(Qt::Horizontal);
    greenSlider->setTickInterval(1);
    greenSlider->setRange(0, 255);
    greenSlider->setValue(0);
    connect(greenSlider, SIGNAL(valueChanged(int)), this, SLOT(greenChanged()));

    blueEdit = new QLineEdit("0 blue");
    blueSlider = new QSlider(Qt::Horizontal);
    blueSlider->setTickInterval(1);
    blueSlider->setRange(0, 255);
    blueSlider->setValue(0);
    connect(blueSlider, SIGNAL(valueChanged(int)), this, SLOT(blueChanged()));

    amberEdit = new QLineEdit("0 amber");
    amberSlider = new QSlider(Qt::Horizontal);
    amberSlider->setTickInterval(1);
    amberSlider->setRange(0, 255);
    amberSlider->setValue(0);
    connect(amberSlider, SIGNAL(valueChanged(int)), this, SLOT(amberChanged()));

    whiteEdit = new QLineEdit("0 white");
    whiteSlider = new QSlider(Qt::Horizontal);
    whiteSlider->setTickInterval(1);
    whiteSlider->setRange(0, 255);
    whiteSlider->setValue(0);
    connect(whiteSlider, SIGNAL(valueChanged(int)), this, SLOT(whiteChanged()));
    
    intensityEdit = new QLineEdit("0 intensity");
    intensitySlider = new QSlider(Qt::Horizontal);
    intensitySlider->setTickInterval(1);
    intensitySlider->setRange(0, 255);
    intensitySlider->setValue(0);
    connect(intensitySlider, SIGNAL(valueChanged(int)), this, SLOT(intensityChanged()));

    zoomEdit = new QLineEdit("0 zoom");
    zoomSlider = new QSlider(Qt::Horizontal);
    zoomSlider->setTickInterval(1);
    zoomSlider->setRange(0, 255);
    zoomSlider->setValue(0);
    connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(zoomChanged()));

    panEdit = new QLineEdit("0° pan");
    panSlider = new QSlider(Qt::Horizontal);
    panSlider->setTickInterval(1);
    panSlider->setRange(-270, 270);
    panSlider->setValue(0);
    connect(panSlider, SIGNAL(valueChanged(int)), this, SLOT(panChanged()));

    tiltEdit = new QLineEdit("0° tilt");
    tiltSlider = new QSlider(Qt::Horizontal);
    tiltSlider->setTickInterval(1);
    tiltSlider->setRange(-110, 110);
    tiltSlider->setValue(0);
    connect(tiltSlider, SIGNAL(valueChanged(int)), this, SLOT(tiltChanged()));

    QHBoxLayout* config = new QHBoxLayout();
    config->setSpacing(5);
    config->addWidget(labelDevice);
    config->addWidget(device);
    config->addWidget(labelIp);
    config->addWidget(ipICE);
    config->addWidget(connectToICE);

    QHBoxLayout* timeLayout = new QHBoxLayout();
    timeLayout->setSpacing(5);
    timeLayout->addWidget(timeEdit, 1);
    timeLayout->addWidget(timeSlider, 5);

    QHBoxLayout* redLayout = new QHBoxLayout();
    redLayout->setSpacing(5);
    redLayout->addWidget(redEdit, 1);
    redLayout->addWidget(redSlider, 5);
    QHBoxLayout* greenLayout = new QHBoxLayout();
    greenLayout->setSpacing(5);
    greenLayout->addWidget(greenEdit, 1);
    greenLayout->addWidget(greenSlider, 5);
    QHBoxLayout* blueLayout = new QHBoxLayout();
    blueLayout->setSpacing(5);
    blueLayout->addWidget(blueEdit, 1);
    blueLayout->addWidget(blueSlider, 5);
    QHBoxLayout* amberLayout = new QHBoxLayout();
    amberLayout->setSpacing(5);
    amberLayout->addWidget(amberEdit, 1);
    amberLayout->addWidget(amberSlider, 5);
    QHBoxLayout* whiteLayout = new QHBoxLayout();
    whiteLayout->setSpacing(5);
    whiteLayout->addWidget(whiteEdit, 1);
    whiteLayout->addWidget(whiteSlider, 5);

    QHBoxLayout* intensityLayout = new QHBoxLayout();
    intensityLayout->setSpacing(5);
    intensityLayout->addWidget(intensityEdit, 1);
    intensityLayout->addWidget(intensitySlider, 5);
    QHBoxLayout* zoomLayout = new QHBoxLayout();
    zoomLayout->setSpacing(5);
    zoomLayout->addWidget(zoomEdit, 1);
    zoomLayout->addWidget(zoomSlider, 5);
    QHBoxLayout* panLayout = new QHBoxLayout();
    panLayout->setSpacing(5);
    panLayout->addWidget(panEdit, 1);
    panLayout->addWidget(panSlider, 5);
    QHBoxLayout* tiltLayout = new QHBoxLayout();
    tiltLayout->setSpacing(5);
    tiltLayout->addWidget(tiltEdit, 1);
    tiltLayout->addWidget(tiltSlider, 5);

    QVBoxLayout* global = new QVBoxLayout();
    global->addItem(config);
    global->addItem(timeLayout);
    global->addSpacing(20);
    global->addItem(redLayout);
    global->addItem(greenLayout);
    global->addItem(blueLayout);
    global->addItem(amberLayout);
    global->addItem(whiteLayout);
    global->addSpacing(20);
    global->addItem(intensityLayout);
    global->addItem(zoomLayout);
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

void CALIB::frequencyChanged()
{
    if (timer->isActive()) {
        timer->stop();
        int value = timeSlider->value();
        timeEdit->setText(QString::number(value) + " ms");
        timer->setInterval(value);
        timer->start();
    }
    else {
        int value = timeSlider->value();
        timeEdit->setText(QString::number(value) + " ms");
        timer->setInterval(value);
    }
}
void CALIB::redChanged() 
{
    double value = redSlider->value();
    redEdit->setText(QString::number(value) + " red");
    render->changeRed(redSlider->value());
}
void CALIB::greenChanged()
{
    double value = greenSlider->value();
    greenEdit->setText(QString::number(value) + " green");
    render->changeGreen(greenSlider->value());
}
void CALIB::blueChanged()
{
    double value = blueSlider->value();
    blueEdit->setText(QString::number(value) + " blue");
    render->changeBlue(blueSlider->value());
}
void CALIB::amberChanged()
{
    double value = amberSlider->value();
    amberEdit->setText(QString::number(value) + " amber");
    render->changeAmber(value);
}
void CALIB::whiteChanged()
{
    double value = whiteSlider->value();
    whiteEdit->setText(QString::number(value) + " white");
    render->changeWhite(whiteSlider->value());
}
void CALIB::intensityChanged()
{
    int value = intensitySlider->value();
    intensityEdit->setText(QString::number(value) + " intensity");
    render->changeIntensity(value);
}
void CALIB::zoomChanged()
{
    int value = zoomSlider->value();
    zoomEdit->setText(QString::number(value) + " zoom");
    render->changeZoom(value);
}
void CALIB::panChanged()
{
    int value = panSlider->value();
    panEdit->setText(QString::number(value) + "° pan");
    render->changePan(value);
}
void CALIB::tiltChanged()
{
    int value = tiltSlider->value();
    tiltEdit->setText(QString::number(value) + "° tilt");
    render->changeTilt(value);
}