#include "calib.h"

CALIB::CALIB(QWidget *parent) :
    QWidget(parent)
{
    //Poco::Logger::root().setChannel(new Poco::ConsoleChannel());

    std::string pathLog = "ICE-CALIB.log";
    Poco::Channel* pChannel = new Poco::FileChannel();
    pChannel->setProperty("path", pathLog);
    pChannel->setProperty("rotation", "500 M");
    pChannel->setProperty("archive", "timestamp");
    pChannel->setProperty("times", "local");
    pChannel->setProperty("compress", "true");
    pChannel->setProperty("purgeCount", "5");

    // -- format output of log
    Poco::PatternFormatter* pattern_formatter = new Poco::PatternFormatter("%Y-%m-%d %H:%M:%S %U:%u %s: %t");
    Poco::FormattingChannel* formatter_channel = new Poco::FormattingChannel(pattern_formatter, pChannel);
    
    // -- initialize log as asynchronous channel (do not take too much process)
    Poco::AsyncChannel* pSync = new Poco::AsyncChannel(formatter_channel);
    try {
       Poco::Logger::root().setChannel(pSync);
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    Poco::Logger::root().setLevel(Poco::Message::PRIO_DEBUG);

    // -- ICE
    labelDevice = new QLabel("Device name");
    device = new QLineEdit("L1");
    connect(device, SIGNAL(editingFinished()), this, SLOT(deviceChanged()));
    labelIp = new QLabel("IP ICE");
    ipICE = new QLineEdit("");
    connectToICE = new QPushButton("Connect ICE");
    connect(connectToICE, SIGNAL(released()), this, SLOT(connectICE()));

    // -- IMS
    labelIpIms = new QLabel("IP IMS");
    ipIMS = new QLineEdit("");
    labelUser = new QLabel("User");
    user = new QLineEdit("manager");
    labelPass = new QLabel("Pass");
    pass = new QLineEdit("r");
    connectToIMS = new QPushButton("Connect IMS");
    connect(connectToIMS, SIGNAL(released()), this, SLOT(connectIMS()));

    load = new QPushButton(QIcon("images/add.png"), "");
    load->setIconSize(QSize(100, 100));
    connect(load, SIGNAL(released()), this, SLOT(loadLUT()));
    save = new QPushButton(QIcon("images/save.png"), "");
    save->setIconSize(QSize(100, 100));
    connect(save, SIGNAL(released()), this, SLOT(saveLUT()));
    renderColor = new QWidget();
    renderColor->setMinimumWidth(100);
    renderColor->setMinimumHeight(100);
    renderColor->resize(100, 100);
    renderColor->setStyleSheet("background-color: rgb(0, 0, 0);");
    mixerMode = new QCheckBox("Mix mode");

    tab = new QTabWidget();
    tab->resize(1280, 620);
    tab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    lut = new LUT();

    // -- initialize red correction
    redGraph = new Graph(16, lut->getRed(), lut->getRedFinalPoly());
    redGraph->setColorPoint(new QColor(255, 0, 0));
    tab->addTab(redGraph, "RED");
    // -- initialize green correction
    greenGraph = new Graph(16, lut->getGreen(), lut->getGreenFinalPoly());
    greenGraph->setColorPoint(new QColor(0, 255, 0));
    tab->addTab(greenGraph, "GREEN");
    // -- initialize blue correction
    blueGraph = new Graph(16, lut->getBlue(), lut->getBlueFinalPoly());
    blueGraph->setColorPoint(new QColor(0, 0, 255));
    tab->addTab(blueGraph, "BLUE");

    // -- initialize red on green correction
    redOnGreenGraph = new Graph(16, lut->getRedOnGreen(), lut->getRedOnGreenFinalPoly());
    redOnGreenGraph->setColorPoint(new QColor(128, 255, 0));
    tab->addTab(redOnGreenGraph, "RED->GREEN");
    // -- initialize green on red correction
    greenOnRedGraph = new Graph(16, lut->getGreenOnRed(), lut->getGreenOnRedFinalPoly());
    greenOnRedGraph->setColorPoint(new QColor(255, 128, 0));
    tab->addTab(greenOnRedGraph, "GREEN->RED");
    // -- initialize blue on green correction
    blueOnGreenGraph = new Graph(16, lut->getBlueOnGreen(), lut->getBlueOnGreenFinalPoly());
    blueOnGreenGraph->setColorPoint(new QColor(0, 255, 128));
    tab->addTab(blueOnGreenGraph, "BLUE->GREEN");
    // -- initialize green on blue correction
    greenOnBlueGraph = new Graph(16, lut->getGreenOnBlue(), lut->getGreenOnBlueFinalPoly());
    greenOnBlueGraph->setColorPoint(new QColor(0, 128, 255));
    tab->addTab(greenOnBlueGraph, "GREEN->BLUE");
    // -- initialize red on blue correction
    redOnBlueGraph = new Graph(16, lut->getRedOnBlue(), lut->getRedOnBlueFinalPoly());
    redOnBlueGraph->setColorPoint(new QColor(128, 0, 255));
    tab->addTab(redOnBlueGraph, "RED->BLUE");
    // -- initialize blue on red correction
    blueOnRedGraph = new Graph(16, lut->getBlueOnRed(), lut->getBlueOnRedFinalPoly());
    blueOnRedGraph->setColorPoint(new QColor(255, 0, 128));
    tab->addTab(blueOnRedGraph, "BLUE->RED");

    QHBoxLayout* iceLayout = new QHBoxLayout();
    iceLayout->setSpacing(5);
    iceLayout->addWidget(labelDevice);
    iceLayout->addWidget(device);
    iceLayout->addWidget(labelIp);
    iceLayout->addWidget(ipICE);
    iceLayout->addWidget(connectToICE);

    QHBoxLayout* imsLayout = new QHBoxLayout();
    imsLayout->setSpacing(5);
    imsLayout->addWidget(labelIpIms);
    imsLayout->addWidget(ipIMS);
    imsLayout->addWidget(labelUser);
    imsLayout->addWidget(user);
    imsLayout->addWidget(labelPass);
    imsLayout->addWidget(pass);
    imsLayout->addWidget(connectToIMS);

    QHBoxLayout* config = new QHBoxLayout();
    config->setSpacing(5);
    config->addWidget(load, 3);
    config->addWidget(save, 3);
    config->addWidget(renderColor, 3);
    config->addWidget(mixerMode, 1);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addLayout(iceLayout, 1);
    globalLayout->addLayout(imsLayout, 1);
    globalLayout->addLayout(config, 1);
    globalLayout->addSpacing(2);
    globalLayout->addWidget(tab, 15);
    setLayout(globalLayout);
    
    render = new Render(lut, renderColor);

    connect(redGraph, SIGNAL(resetEmission(int, double, double)), this, SLOT(redrawPoly(int, double, double)));
    connect(greenGraph, SIGNAL(resetEmission(int, double, double)), this, SLOT(redrawPoly(int, double, double)));
    connect(blueGraph, SIGNAL(resetEmission(int, double, double)), this, SLOT(redrawPoly(int, double, double)));
    connect(redOnGreenGraph, SIGNAL(resetEmission(int, double, double)), this, SLOT(redrawPoly(int, double, double)));
    connect(blueOnGreenGraph, SIGNAL(resetEmission(int, double, double)), this, SLOT(redrawPoly(int, double, double)));
    connect(redOnBlueGraph, SIGNAL(resetEmission(int, double, double)), this, SLOT(redrawPoly(int, double, double)));
    connect(greenOnBlueGraph, SIGNAL(resetEmission(int, double, double)), this, SLOT(redrawPoly(int, double, double)));
    connect(greenOnRedGraph, SIGNAL(resetEmission(int, double, double)), this, SLOT(redrawPoly(int, double, double)));
    connect(blueOnRedGraph, SIGNAL(resetEmission(int, double, double)), this, SLOT(redrawPoly(int, double, double)));

    timer = new QTimer();
    timer->setTimerType(Qt::PreciseTimer);
    timer->setInterval(30);
    connect(timer, SIGNAL(timeout()), render, SLOT(run()));
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

void CALIB::connectIMS() 
{
    QString ip = ipIMS->text();
    if (!ip.isEmpty()) {
        render->connectIMS(ip.toStdString(), user->text().toStdString(), pass->text().toStdString());
    }
}

void CALIB::loadLUT()
{
    timer->stop();
    QString file = QFileDialog::getOpenFileName(this, "Choose lut file to open", QString(), "*.lut");
    if (!file.isEmpty()) {
        std::cout << "File selected is " << file.toUtf8().constData();
        lut->open(file.toUtf8().constData());
    }
    timer->start();
}

void CALIB::saveLUT()
{
    timer->stop();
    QString file = QFileDialog::getSaveFileName(this, "Choose where to save lut file", QString(), "*.lut");
    if (!file.isEmpty()) {
        std::cout << "File selected is " << file.toUtf8().constData() << std::endl;
        lut->save(file.toUtf8().constData());
    }
    timer->start();
}

void CALIB::redrawPoly(int index, double oldValue, double newValue)
{
    Graph* graph = qobject_cast<Graph*>(sender());

    if (mixerMode->isChecked()) {
        if (graph == redGraph) {

            double ratioGreen = lut->getRedOnGreen()[index]/oldValue;
            std::cout << "red graph " << index << " " << oldValue << " " << newValue << std::endl;
            std::cout << "red graph " << ratioGreen << " " << lut->getRedOnGreen()[index] << " " << newValue*ratioGreen << std::endl;
            lut->getRedOnGreen()[index] = newValue*ratioGreen;
            redOnGreenGraph->repaint();
        }
        else if (graph == greenGraph) {
            double ratioRed = lut->getGreenOnRed()[index]/oldValue;
            std::cout << "green graph " << index << " " << oldValue << " " << newValue << std::endl;
            std::cout << "green graph " << ratioRed << " " << lut->getGreenOnRed()[index] << " " << newValue*ratioRed << std::endl;
            lut->getGreenOnRed()[index] = newValue*ratioRed;
            greenOnRedGraph->repaint();
        }
        else if (graph == blueGraph) {
            double ratioRed = lut->getBlueOnRed()[index]/oldValue;
            std::cout << "blue graph " << index << " " << oldValue << " " << newValue << std::endl;
            std::cout << "blue graph " << ratioRed << " " << lut->getBlueOnRed()[index] << " " << newValue*ratioRed << std::endl;
            lut->getBlueOnRed()[index] = newValue*ratioRed;
            blueOnRedGraph->repaint();
        }
    }

    lut->recalculateAll();
    render->resetEmission();
    graph->repaint();
}