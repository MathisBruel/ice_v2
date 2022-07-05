#include "generateSyncWidget.h"

GenerateSync::GenerateSync(QWidget *parent)
    : QWidget(parent)
{

    folder = "";
    cplRef = nullptr;
    cis = nullptr;

    loadCPL = new QPushButton(QIcon("images/add.png"), "");
    loadCPL->setIconSize(QSize(50, 50));
    connect(loadCPL, SIGNAL(released()), this, SLOT(load()));
    loadCIS = new QPushButton(QIcon("images/cis.png"), "");
    loadCIS->setIconSize(QSize(50, 50));
    connect(loadCIS, SIGNAL(released()), this, SLOT(loadCis()));
    outputPath = new QPushButton(QIcon("images/save.png"), "");
    outputPath->setIconSize(QSize(50, 50));
    connect(outputPath, SIGNAL(released()), this, SLOT(output()));
    generate = new QPushButton(QIcon("images/generate.png"), "");
    generate->setIconSize(QSize(50, 50));
    connect(generate, SIGNAL(released()), this, SLOT(genSync()));
    savePath = new QLineEdit("");
    savePath->setReadOnly(true);
    cisPath = new QLineEdit("");
    savePath->setReadOnly(true);

    idLabel = new QLabel("ID");
    id = new QLineEdit("");
    id->setReadOnly(true);
    titleLabel = new QLabel("TITLE");
    title = new QLineEdit("");
    title->setReadOnly(true);
    durationLabel = new QLabel("DURATION");
    duration = new QLineEdit("");
    duration->setReadOnly(true);
    speedRateLabel = new QLabel("SPEEDRATE");
    speedRate = new QLineEdit("");
    speedRate->setReadOnly(true);
    
    startDelayLabel = new QLabel("START DELAY");
    startDelay = new QLineEdit("0");
    startDelay->setReadOnly(true);
    startDelaySlider = new QSlider(Qt::Horizontal);
    startDelaySlider->setMinimum(0);
    startDelaySlider->setMaximum(0);
    connect(startDelaySlider, SIGNAL(valueChanged(int)), this, SLOT(startDelayChanged(int)));
    entryPointLabel = new QLabel("ENTRY POINT");
    entryPoint = new QLineEdit("0");
    entryPoint->setReadOnly(true);
    entryPointSlider = new QSlider(Qt::Horizontal);
    entryPointSlider->setMinimum(0);
    entryPointSlider->setMaximum(0);
    connect(entryPointSlider, SIGNAL(valueChanged(int)), this, SLOT(entryPointChanged(int)));

    iceVersion = new QComboBox();
    iceVersion->addItem("V1", 1);
    iceVersion->addItem("V2", 2);
    connect(iceVersion, SIGNAL(currentIndexChanged(int)), this, SLOT(versionChanged(int)));
    loop = new QCheckBox("");
    loop->setIcon(QIcon("images/loop.png"));
    loop->setIconSize(QSize(50, 50));
    connect(loop, SIGNAL(stateChanged(int)), this, SLOT(loopChanged(int)));

    projection = new QComboBox();
    projection->addItem("Unknow", 6);
    projection->addItem("NO FIT", 0);
    projection->addItem("FIT Y", 1);
    projection->addItem("FIT X", 2);
    projection->addItem("FIT Y (proportion applied to X)", 3);
    projection->addItem("FIT X (proportion applied to Y)", 4);
    projection->addItem("FIT X/Y", 5);

    options = new QTabWidget();
    startCommandList = new CommandList();
    options->addTab(startCommandList, "START CMD");
    endCommandList = new CommandList();
    options->addTab(endCommandList, "END CMD");

    viewer = new SyncViewer(this);
    viewer->hide();

    QVBoxLayout* pathLayout = new QVBoxLayout();
    pathLayout->addWidget(savePath);
    pathLayout->addWidget(cisPath);

    QHBoxLayout* fileControlLayout = new QHBoxLayout();
    fileControlLayout->addWidget(loadCPL);
    fileControlLayout->addWidget(loadCIS);
    fileControlLayout->addWidget(outputPath);
    fileControlLayout->addWidget(generate);
    fileControlLayout->addSpacing(5);
    fileControlLayout->addWidget(loop);
    fileControlLayout->addSpacing(5);
    fileControlLayout->addItem(pathLayout);

    QGridLayout* infoCplLayout = new QGridLayout();
    infoCplLayout->addWidget(idLabel, 0, 0);
    infoCplLayout->addWidget(id, 0, 1, 1, 3);
    infoCplLayout->addWidget(titleLabel, 1, 0);
    infoCplLayout->addWidget(title, 1, 1, 1, 3);
    infoCplLayout->addWidget(durationLabel, 2, 0);
    infoCplLayout->addWidget(duration, 2, 1);
    infoCplLayout->addWidget(speedRateLabel, 2, 2);
    infoCplLayout->addWidget(speedRate, 2, 3);
    infoCplLayout->setHorizontalSpacing(5);
    infoCplLayout->setVerticalSpacing(2);

    QGridLayout* sliderLayout = new QGridLayout();
    sliderLayout->addWidget(startDelayLabel, 0, 0, 1, 1);
    sliderLayout->addWidget(startDelay, 0, 1, 1, 3);
    sliderLayout->addWidget(startDelaySlider, 1, 0, 1, 4);
    sliderLayout->addWidget(entryPointLabel, 0, 4, 1, 1);
    sliderLayout->addWidget(entryPoint, 0, 5, 1, 3);
    sliderLayout->addWidget(entryPointSlider, 1, 4, 1, 4);
    sliderLayout->setHorizontalSpacing(5);
    sliderLayout->setVerticalSpacing(2);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addItem(fileControlLayout);
    globalLayout->addSpacing(10);
    globalLayout->addItem(infoCplLayout);
    globalLayout->addItem(sliderLayout);
    globalLayout->addWidget(iceVersion);
    globalLayout->addSpacing(10);
    globalLayout->addWidget(viewer);
    globalLayout->addWidget(projection, 0, Qt::AlignLeft);
    globalLayout->addWidget(options);
    globalLayout->addStretch();
    setLayout(globalLayout);

    showDataInfo(false);
}

GenerateSync::~GenerateSync()
{
    
}

void GenerateSync::showDataInfo(bool show)
{
    if (show) {
        idLabel->show();
        id->show();
        titleLabel->show();
        title->show();
        durationLabel->show();
        duration->show();
        speedRateLabel->show();
        speedRate->show();
        startDelayLabel->show();
        startDelay->show();
        startDelaySlider->show();
        entryPointLabel->show();
        entryPoint->show();
        entryPointSlider->show();
        iceVersion->show();
        loop->show();
        projection->show();
        options->show();
        versionChanged(0);
    }
    else {
        idLabel->hide();
        id->hide();
        titleLabel->hide();
        title->hide();
        durationLabel->hide();
        duration->hide();
        speedRateLabel->hide();
        speedRate->hide();
        startDelayLabel->hide();
        startDelay->hide();
        startDelaySlider->hide();
        entryPointLabel->hide();
        entryPoint->hide();
        entryPointSlider->hide();
        iceVersion->hide();
        loop->hide();
        projection->hide();
        options->hide();
    }
}
void GenerateSync::showDataV2(bool show)
{
    if (show) {
        projection->show();
        options->show();
    }
    else {
        projection->hide();
        options->hide();
    }
}

void GenerateSync::load()
{
    QString file = QFileDialog::getOpenFileName(this, "Choose CPL file to synchronize", QString(), "*.xml");
    if (!file.isEmpty()) {
        CplFile* cplNew = new CplFile(file.toUtf8().constData());
        
        if (!cplNew->load()) {
            QMessageBox errorWindow;
            errorWindow.setText("Error while loading CPL : " + file);
            errorWindow.exec();
            viewer->hide();
        }
        else {

            idCpl = QString::fromUtf8(cplNew->getCplId().c_str());
            id->setText(idCpl);
            QString titleStr = QString::fromUtf8(cplNew->getCplTitle().c_str());
            title->setText(titleStr);
            double speedRateValue = cplNew->getReels().at(0).speedRate;
            QString speedRateStr = QString::number(speedRateValue);
            speedRate->setText(speedRateStr);

            double durationMs = cplNew->getTotalMsDuration(nullptr);
            int nbFrame = durationMs*speedRateValue/1000;
            QString durationStr = QString::number(nbFrame);
            duration->setText(durationStr);

            showDataInfo(true);
            savePath->setText(folder + "/" + idCpl + ".sync");

            viewer->setData(cis, cplNew, nullptr);

            if (cplRef != nullptr) {
                delete cplRef;
                cplRef = nullptr;
            }

            cplRef = cplNew;
            
            // -- draw viewer synchro
            if (cis != nullptr) {
                viewer->show();
            }
        }
    }
}

void GenerateSync::loadCis()
{
    QString file = QFileDialog::getOpenFileName(this, "Choose CIS file to synchronize", QString(), "*.cis");
    if (!file.isEmpty()) {

        Cis* cisNew = new Cis();
        if (!cisNew->open(file.toUtf8().constData())) {
            QMessageBox errorWindow;
            errorWindow.setText("Error while opening : " + file);
            errorWindow.exec();
            viewer->hide();
            return;
        }

        cisPath->setText(file);
        cisFile = file.toUtf8().constData();

        startDelaySlider->setMaximum(cisNew->getSpeedRate()*60);
        entryPointSlider->setMaximum(cisNew->getSpeedRate()*60);

        viewer->setData(cisNew, cplRef, nullptr);

        if (cis != nullptr) {
            delete cis;
            cis = nullptr;
        }
        cis = cisNew;

        if (cplRef != nullptr) {
            viewer->show();
        }
    }
}

void GenerateSync::output()
{
    QString folderTemp = QFileDialog::getExistingDirectory(this, tr("Choose output directory"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!folderTemp.isEmpty()) {
        folder = folderTemp;
        savePath->setText(folder + "/" + idCpl + ".sync");
    }
}

void GenerateSync::genSync()
{
    if (folder.isEmpty()) {
        QMessageBox errorWindow;
        errorWindow.setText("Output folder is empty => set it before generation !");
        errorWindow.exec();
        return;
    }

    Poco::Path folderOut(folder.toUtf8().constData());
    if (folderOut.getExtension() != "lcp") {
        QMessageBox errorWindow;
        errorWindow.setText("Output folder must have the extension '.lcp' !");
        errorWindow.exec();
        return;
    }
    
    QString filePath = folder + "/" + idCpl + ".sync";
    Poco::File fileToCopy(cisFile);
    std::string sha1Cis = Converter::calculateSha1OfFile(cisFile);
    Poco::Path pathLvi(cisFile);
    QString lviFilePath = folder + "/" + QString::fromUtf8(sha1Cis.c_str()) + ".lvi";
    QString cisFilePath = folder + "/" + QString::fromUtf8(sha1Cis.c_str()) + ".cis";
    fileToCopy.copyTo(cisFilePath.toUtf8().constData());
    SyncFile sync(filePath.toUtf8().constData());
    LVI lvi(lviFilePath.toUtf8().constData());

    if (iceVersion->currentData().toInt() == 1) {
        sync.setVersion("1.2");
        lvi.setVersion("1.2");
    }
    else {
        sync.setVersion("2.0");
        lvi.setVersion("2.0");
    }
    sync.setType("imagesequence");
    lvi.setType("imagesequence");

    if (loop->isChecked()) {
        lvi.setLoop(true);
    } else {
        lvi.setLoop(false);
    }

    sync.setSha1(sha1Cis);
    lvi.setSha1(sha1Cis);
    lvi.setName(folderOut.getBaseName());
    lvi.setDuration(cis->getFrameCount());

    sync.setCplId(cplRef->getCplId());
    sync.setCplTitle(cplRef->getCplTitle());
    lvi.setCplId(cplRef->getCplId());
    lvi.setCplTitle(cplRef->getCplTitle());

    double speedRateValue = cplRef->getReels().at(0).speedRate;
    double durationMs = cplRef->getTotalMsDuration(nullptr);
    int nbFrameCpl = durationMs*speedRateValue/1000;

    sync.setCplDuration(nbFrameCpl);
    lvi.setCplDuration(nbFrameCpl);

    sync.setStartValues(startDelaySlider->sliderPosition(), entryPointSlider->sliderPosition());
    lvi.setStartValues(startDelaySlider->sliderPosition(), entryPointSlider->sliderPosition());
    sync.setSpeedRate(speedRateValue);
    sync.setEditRate(cplRef->getReels().at(0).editRate);
    lvi.setEditRate(cplRef->getReels().at(0).editRate);

    // -- reels
    for (int i = 0; i < cplRef->getReels().size(); i++) {
        sync.addReel(cplRef->getReels().at(i));
        lvi.addReel(cplRef->getReels().at(i));
    }

    sync.setTypeProjection(static_cast<PanelProjection::ProjectionType>(projection->currentData().toInt()));
    sync.setCuts(nullptr);

    for (int i = 0; i < startCommandList->getTable()->rowCount(); i++) {
        Command::CommandType type = Command::generateFromString(startCommandList->getTable()->item(i, 0)->text().toUtf8().constData());
        std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(type));
        if (type == Command::SWITCH_MODE) {
            std::string paramValue = startCommandList->getTable()->item(i, 1)->text().toUtf8().constData();
            cmd->addParameter("mode", paramValue);
        }
        else if (type == Command::LAUNCH_SCRIPT) {
            std::string paramValue = startCommandList->getTable()->item(i, 1)->text().toUtf8().constData();
            cmd->addParameter("force", paramValue);
            Poco::Path scriptDirectory(folder.toUtf8().constData());
            cmd->addParameter("script", scriptDirectory.getFileName());
        }
        else if (type == Command::SWITCH_PROJECTION) {
            std::string paramValue = startCommandList->getTable()->item(i, 1)->text().toUtf8().constData();
            cmd->addParameter("projection", paramValue);
        }
        sync.addStartCommand(cmd);
    }

    for (int i = 0; i < endCommandList->getTable()->rowCount(); i++) {
        Command::CommandType type = Command::generateFromString(endCommandList->getTable()->item(i, 0)->text().toUtf8().constData());
        std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(type));
        if (type == Command::SWITCH_MODE) {
            std::string paramValue = endCommandList->getTable()->item(i, 1)->text().toUtf8().constData();
            cmd->addParameter("mode", paramValue);
        }
        else if (type == Command::LAUNCH_SCRIPT) {
            std::string paramValue = endCommandList->getTable()->item(i, 1)->text().toUtf8().constData();
            cmd->addParameter("force", paramValue);
            Poco::Path scriptDirectory(folder.toUtf8().constData());
            cmd->addParameter("script", scriptDirectory.getFileName());
        }
        else if (type == Command::SWITCH_PROJECTION) {
            std::string paramValue = endCommandList->getTable()->item(i, 1)->text().toUtf8().constData();
            cmd->addParameter("projection", paramValue);
        }
        sync.addEndCommand(cmd);
    }

    sync.save();
    Poco::Logger::get("GenerateSync").debug("Sync file saved !");
    lvi.save();
    Poco::Logger::get("GenerateSync").debug("Lvi file saved !");

    QMessageBox infoWindow;
    infoWindow.setText("Generation success !");
    infoWindow.exec();
}

void GenerateSync::versionChanged(int index)
{
    if (iceVersion->currentData() == 1) {
        showDataV2(false);
    }
    else if (iceVersion->currentData() == 2) {
        showDataV2(true);
    }
}

void GenerateSync::startDelayChanged(int value)
{
    startDelay->setText(QString::number(value));
    viewer->setTimings(entryPointSlider->sliderPosition(), startDelaySlider->sliderPosition());
}
void GenerateSync::entryPointChanged(int value)
{
    entryPoint->setText(QString::number(value));
    viewer->setTimings(entryPointSlider->sliderPosition(), startDelaySlider->sliderPosition());
}

void GenerateSync::loopChanged(int state)
{
    if (state == Qt::Checked) {
        viewer->setLoop(true);
    }
    else {
        viewer->setLoop(false);
    }
}

void GenerateSync::updateViewer()
{
    viewer->repaint();
}