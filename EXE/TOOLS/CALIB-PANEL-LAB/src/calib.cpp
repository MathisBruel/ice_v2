#include "calib.h"

CALIB::CALIB(QWidget *parent) :
    QWidget(parent)
{
    Poco::Logger::root().setChannel(new Poco::ConsoleChannel());
    Poco::Logger::root().setLevel(Poco::Message::PRIO_DEBUG);

    load = new QPushButton(QIcon("images/add.png"), "");
    load->setIconSize(QSize(100, 100));
    connect(load, SIGNAL(released()), this, SLOT(loadLAB()));

    save = new QPushButton(QIcon("images/save.png"), "");
    save->setIconSize(QSize(100, 100));
    connect(save, SIGNAL(released()), this, SLOT(saveLAB()));

    factorAEdit = new QLineEdit("Factor a : 0.0");
    // -- precision of 0.001
    factorA = new QSlider();
    factorA->setMinimum(0);
    factorA->setMaximum(1000);
    factorA->setSingleStep(1);
    factorA->setOrientation(Qt::Horizontal);
    connect(factorA, SIGNAL(valueChanged(int)), this, SLOT(changeFactorA(int)));

    biasAEdit = new QLineEdit("Bias a : 0.0");
    // -- precision of 0.01
    biasA = new QSlider();
    biasA->setMinimum(-2000);
    biasA->setMaximum(2000);
    biasA->setSingleStep(1);
    biasA->setOrientation(Qt::Horizontal);
    connect(biasA, SIGNAL(valueChanged(int)), this, SLOT(changeBiasA(int)));

    factorBEdit = new QLineEdit("Factor b : 0.0");
    // -- precision of 0.001
    factorB = new QSlider();
    factorB->setMinimum(0);
    factorB->setMaximum(1000);
    factorB->setSingleStep(1);
    factorB->setOrientation(Qt::Horizontal);
    connect(factorB, SIGNAL(valueChanged(int)), this, SLOT(changeFactorB(int)));

    biasBEdit = new QLineEdit("Bias b : 0.0");
    // -- precision of 0.01
    biasB = new QSlider();
    biasB->setMinimum(-2000);
    biasB->setMaximum(2000);
    biasB->setSingleStep(1);
    biasB->setOrientation(Qt::Horizontal);
    connect(biasB, SIGNAL(valueChanged(int)), this, SLOT(changeBiasB(int)));

    calib = new LabCalib();
    lumGraph = new Graph(21, calib->getDataDstL());

    QHBoxLayout* line = new QHBoxLayout();
    line->addWidget(load);
    line->addWidget(save);

    QHBoxLayout* lineFactorA = new QHBoxLayout();
    lineFactorA->addWidget(factorAEdit, 1);
    lineFactorA->addSpacing(2);
    lineFactorA->addWidget(factorA, 10);
    QHBoxLayout* lineBiasA = new QHBoxLayout();
    lineBiasA->addWidget(biasAEdit, 1);
    lineBiasA->addSpacing(2);
    lineBiasA->addWidget(biasA, 10);
    QHBoxLayout* lineFactorB = new QHBoxLayout();
    lineFactorB->addWidget(factorBEdit, 1);
    lineFactorB->addSpacing(2);
    lineFactorB->addWidget(factorB, 10);
    QHBoxLayout* lineBiasB = new QHBoxLayout();
    lineBiasB->addWidget(biasBEdit, 1);
    lineBiasB->addSpacing(2);
    lineBiasB->addWidget(biasB, 10);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addItem(line);
    globalLayout->addSpacing(2);
    globalLayout->addItem(lineFactorA);
    globalLayout->addItem(lineBiasA);
    globalLayout->addItem(lineFactorB);
    globalLayout->addItem(lineBiasB);
    globalLayout->addSpacing(2);
    globalLayout->addWidget(lumGraph);
    setLayout(globalLayout);
    
    render = new Render(calib);
    render->init();

    connect(lumGraph, SIGNAL(resetEmission()), render, SLOT(resetEmission()));

    timer = new QTimer();
    timer->setTimerType(Qt::PreciseTimer);
    timer->setInterval(30);
    connect(timer, SIGNAL(timeout()), render, SLOT(run()));
    timer->start();
}

CALIB::~CALIB()
{
    
}

void CALIB::loadLAB()
{
    timer->stop();
    QString file = QFileDialog::getOpenFileName(this, "Choose lab file to open", QString(), "*.lab");
    if (!file.isEmpty()) {
        std::cout << "File selected is " << file.toUtf8().constData();
        calib->open(file.toUtf8().constData());
    }

    factorA->setSliderPosition(calib->getFactorA()*1000);
    factorB->setSliderPosition(calib->getFactorB()*1000);
    biasA->setSliderPosition(calib->getBiasA()*100);
    biasB->setSliderPosition(calib->getBiasB()*100);
    timer->start();
}

void CALIB::saveLAB()
{
    timer->stop();
    QString file = QFileDialog::getSaveFileName(this, "Choose where to save lab file", QString(), "*.lab");
    if (!file.isEmpty()) {
        std::cout << "File selected is " << file.toUtf8().constData() << std::endl;
        calib->save(file.toUtf8().constData());
    }
    timer->start();
}

void CALIB::changeFactorA(int value)
{
    double valueF = (double)value / 1000.0;
    calib->setFactorA(valueF);
    QString str = "Factor a : " + QString::number(valueF);
    factorAEdit->setText(str);
    render->resetEmission();
}
void CALIB::changeBiasA(int value)
{
    double valueF = (double)value / 100.0;
    calib->setBiasA(valueF);
    QString str = "Bias a : " + QString::number(valueF);
    biasAEdit->setText(str);
    render->resetEmission();
}
void CALIB::changeFactorB(int value)
{
    double valueF = (double)value / 1000.0;
    calib->setFactorB(valueF);
    QString str = "Factor b : " + QString::number(valueF);
    factorBEdit->setText(str);
    render->resetEmission();
}
void CALIB::changeBiasB(int value)
{
    double valueF = (double)value / 100.0;
    calib->setBiasB(valueF);
    QString str = "Bias b : " + QString::number(valueF);
    biasBEdit->setText(str);
    render->resetEmission();
}