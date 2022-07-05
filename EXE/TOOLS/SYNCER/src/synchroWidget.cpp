#include "synchroWidget.h"

SynchroWidget::SynchroWidget(QWidget *parent)
    : QWidget(parent)
{
    inSync = nullptr;
    cplRef = nullptr;
    cuts = nullptr;
    synchroEngine = nullptr;

    folder = QString();

    addSync = new QPushButton(QIcon("images/addCpl.png"), "");
    addSync->setIconSize(QSize(50, 50));
    connect(addSync, SIGNAL(released()), this, SLOT(addSyncFile()));
    addCpl = new QPushButton(QIcon("images/add.png"), "");
    addCpl->setIconSize(QSize(50, 50));
    connect(addCpl, SIGNAL(released()), this, SLOT(addCplFile()));
    outputSync = new QPushButton(QIcon("images/save.png"), "");
    outputSync->setIconSize(QSize(50, 50));
    connect(outputSync, SIGNAL(released()), this, SLOT(setOutputDir()));
    synchronize = new QPushButton(QIcon("images/generate.png"), "");
    synchronize->setIconSize(QSize(50, 50));
    connect(synchronize, SIGNAL(released()), this, SLOT(launchSync()));
    genSubtiles = new QPushButton(QIcon("images/subtiles.png"), "");
    genSubtiles->setIconSize(QSize(50, 50));
    connect(genSubtiles, SIGNAL(released()), this, SLOT(generateSubtiles()));
    genSubtiles->setEnabled(false);

    modeSync = new QComboBox();
    modeSync->addItem("Id", 1);
    modeSync->addItem("Duration", 2);

    nameOfSync = new QLineEdit("");
    nameOfSync->setReadOnly(true);
    nameOfCpl = new QLineEdit("");
    nameOfCpl->setReadOnly(true);

    viewer = new SynchroViewer(this);
    viewer->hide();

    options = new QTabWidget();
    cuts = new CutScenes("", "", "", ""); 
    cutsList = new CutSceneList(cuts);
    options->addTab(cutsList, "CUTS");
    startCommandList = new CommandList();
    options->addTab(startCommandList, "START CMD");
    endCommandList = new CommandList();
    options->addTab(endCommandList, "END CMD");

    QVBoxLayout* pathLayout = new QVBoxLayout();
    pathLayout->addWidget(nameOfSync);
    pathLayout->addWidget(nameOfCpl);

    QHBoxLayout* fileControlLayout = new QHBoxLayout();
    fileControlLayout->addWidget(addSync);
    fileControlLayout->addWidget(addCpl);
    fileControlLayout->addWidget(outputSync);
    fileControlLayout->addWidget(synchronize);
    fileControlLayout->addWidget(genSubtiles);
    fileControlLayout->addSpacing(5);
    fileControlLayout->addWidget(modeSync);
    fileControlLayout->addSpacing(5);
    fileControlLayout->addItem(pathLayout);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addItem(fileControlLayout);
    globalLayout->addSpacing(10);
    globalLayout->addWidget(options);
    globalLayout->addSpacing(10);
    globalLayout->addWidget(viewer);
    globalLayout->addStretch();
    setLayout(globalLayout);
}

SynchroWidget::~SynchroWidget()
{
    
}

void SynchroWidget::addSyncFile()
{
    QString file = QFileDialog::getOpenFileName(this, "Choose Sync file reference", QString(), "*.sync *.xml");
    if (!file.isEmpty()) {

        Synchronizable* newSync = nullptr;

        Poco::Path pathFile(file.toUtf8().constData());
        if (pathFile.getExtension() == "sync") {
            newSync = new SyncFile(file.toUtf8().constData());
        }
        else if (pathFile.getExtension() == "xml") {
            newSync = new CplFile(file.toUtf8().constData());
        }

        if (!newSync->load()) {
            QMessageBox errorWindow;
            errorWindow.setText("Error while loading SYNC file : " + file);
            errorWindow.exec();
        }
        else {

            if (inSync != nullptr) {
                delete inSync;
                inSync = nullptr;
            }

            if (newSync->getCuts() != nullptr) {
                QMessageBox errorWindow;
                errorWindow.setText("Error : you can't use a sync file that has cuts as reference !");
                errorWindow.exec();
            }
            else {
                nameOfSync->setText(QString::fromStdString(newSync->getCplTitle()));
                inSync = newSync;
            }
        }
    }
}

void SynchroWidget::addCplFile()
{
    QString file = QFileDialog::getOpenFileName(this, "Choose CPL file to synchronize", QString(), "*.xml");
    if (!file.isEmpty()) {
        CplFile* cplNew = new CplFile(file.toUtf8().constData());
        
        if (!cplNew->load()) {
            QMessageBox errorWindow;
            errorWindow.setText("Error while loading CPL : " + file);
            errorWindow.exec();
        }
        else {

            nameOfCpl->setText(QString::fromStdString(cplNew->getCplTitle()));

            if (cplRef != nullptr) {
                delete cplRef;
                cplRef = nullptr;
            }

            cplRef = cplNew;
        }
    }
}

void SynchroWidget::setOutputDir()
{
    QString folderTemp = QFileDialog::getExistingDirectory(this, tr("Choose output directory"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!folderTemp.isEmpty()) {
        folder = folderTemp;
    }
}

void SynchroWidget::generateSubtiles()
{
    if (folder.isEmpty()) {
        QMessageBox errorWindow;
        errorWindow.setText("Output directory is not set !");
        errorWindow.exec();
        return;
    }

    if (synchroEngine == nullptr) {
        QMessageBox errorWindow;
        errorWindow.setText("No synchro launched !");
        errorWindow.exec();
        return;
    }

    std::vector<Synchro::InterMatch> interMatches = synchroEngine->getInter();
    double speedRate = inSync->getSpeedRate();
    if (speedRate == -1.0) {
        speedRate = inSync->getReels().at(0).speedRate;
    }

    std::string folderStr = folder.toUtf8().constData();
    Poco::Path pathSubtile(folderStr + "/cuts.txt");
    std::ofstream out_file;
    out_file.open(pathSubtile.toString());
    for (int i = 0; i < interMatches.size(); i++) {
        if (!interMatches.at(i).valid) {

            double start = interMatches.at(i).startMsMatchRef;
            int hourStart = start / 3600000;
            start -= hourStart * 3600000;
            int minuteStart = start / 60000;
            start -= minuteStart * 60000;
            int secondStart = start / 1000;
            start -= secondStart * 1000;
            int frameStart = (int)std::round(start*speedRate);

            double end = interMatches.at(i).endMsMatchRef;
            int hourEnd = end / 3600000;
            end -= hourEnd * 3600000;
            int minuteEnd = end / 60000;
            end -= minuteEnd * 60000;
            int secondEnd = end / 1000;
            end -= secondEnd * 1000;
            int frameEnd = (int)std::round(end*speedRate);

            out_file << (i+1) << std::endl;
            out_file << hourStart << ":" << minuteStart << ":" << secondStart << ":" << frameStart << " - ";
            out_file << hourEnd << ":" << minuteEnd << ":" << secondEnd << ":" << frameEnd << std::endl;
            out_file << "Possible CUT n°" << (i+1) << std::endl << std::endl;
        }
    }
    out_file.close();

    Poco::Path pathCaption(folderStr + "/cuts.xml");
    std::ofstream out_file_caption;
    out_file_caption.open(pathCaption.toString());

    out_file_caption << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << std::endl;
    out_file_caption << "<tt xmlns=\"http://www.w3.org/ns/ttml\" ";
    out_file_caption << "xmlns:ttp=\"http://www.w3.org/ns/ttml#parameter\" ttp:dropMode=\"nonDrop\" ttp:frameRate=\"24\" ttp:timeBase=\"smpte\" ";
    out_file_caption << "xmlns:m708=\"http://www.smpte-ra.org/schemas/2052-1/2013/smpte-tt#cea708\" xmlns:smpte=\"http://www.smpte-ra.org/schemas/2052-1/2010/smpte-tt\" ";
    out_file_caption << "xmlns:ttm=\"http://www.w3.org/ns/ttml#metadata\" xmlns:tts=\"http://www.w3.org/ns/ttml#styling\">" << std::endl;
    out_file_caption << "<head><metadata><smpte:information m708:aspectRatio=\"16:9\" m708:easyReader=\"false\" m708:number=\"1\"/>";
    out_file_caption << "</metadata><styling><style tts:color=\"white\" tts:fontFamily=\"monospace\" tts:fontWeight=\"normal\" tts:textAlign=\"left\" xml:id=\"basic\"/>";
    out_file_caption << "</styling><layout><region tts:backgroundColor=\"transparent\" xml:id=\"pop1\"/></layout></head>" << std::endl;
    out_file_caption << "<body><div>" << std::endl << std::endl;

    for (int i = 0; i < interMatches.size(); i++) {
        if (!interMatches.at(i).valid) {

            double start = interMatches.at(i).startMsMatchRef;
            int hourStart = start / 3600000;
            start -= hourStart * 3600000;
            int minuteStart = start / 60000;
            start -= minuteStart * 60000;
            int secondStart = start / 1000;
            start -= secondStart * 1000;
            int frameStart = (int)std::round(start*speedRate);

            double end = interMatches.at(i).endMsMatchRef;
            int hourEnd = end / 3600000;
            end -= hourEnd * 3600000;
            int minuteEnd = end / 60000;
            end -= minuteEnd * 60000;
            int secondEnd = end / 1000;
            end -= secondEnd * 1000;
            int frameEnd = (int)std::round(end*speedRate);

            out_file_caption << "<p begin=\"" << hourStart << ":" << minuteStart << ":" << secondStart << ":" << frameStart << "\" ";
            out_file_caption << "end=\"" << hourEnd << ":" << minuteEnd << ":" << secondEnd << ":" << frameEnd << "\" ";
            out_file_caption << "region=\"pop1\" style=\"basic\" tts:origin=\"10% 84.666666666666671%\" tts:textAlign=\"center\">";
			out_file_caption << "<span tts:fontFamily=\"Helvetica\" tts:fontSize=\"40px\">" << "Possible CUT n°" << (i+1) << "</span></p>" << std::endl;
        }
    }
    out_file_caption << "</div></body></tt>";
    out_file_caption.close();
}

void SynchroWidget::launchSync()
{
    if (synchroEngine != nullptr) {
        delete synchroEngine;
    }
    synchroEngine = new Synchro(inSync, cplRef, cuts);

    // -- match by PictureId
    if (modeSync->currentData().toInt() == 1) {
        synchroEngine->tryFindSamePictureId();

        std::vector<Synchro::Match2> matches = synchroEngine->getMatch2();

        // -- check at least one match exists
        if (matches.size() <= 0) {
            QMessageBox errorWindow;
            errorWindow.setText("No match on id of Reel ! No synchronization possible !");
            errorWindow.exec();
        }
        else {
            synchroEngine->checkFilled2();
            viewer->setData(synchroEngine, true);
            viewer->show();

            if (synchroEngine->isSynchronized()) {
                QMessageBox textWindow;
                textWindow.setText("Synchronization succeed ! Start delay : " + 
                QString::number(synchroEngine->getStartDelay()) + ", EntryPoint : " + 
                QString::number(synchroEngine->getEntryPoint()));
                textWindow.exec();

                if (folder.isEmpty()) {
                    QMessageBox errorWindow;
                    errorWindow.setText("Output directory not specified !");
                    errorWindow.exec();
                }
                else {
                    std::string file = folder.toUtf8().constData();
                    file += "/";
                    file += cplRef->getCplId();
                    file += ".sync";
                    std::cout << "Output file " << file << std::endl;
                    SyncFile* syncFile = synchroEngine->generateSyncFile(file);
                    syncFile->save();
                    delete syncFile;
                }
            }
            else {
                QMessageBox errorWindow;
                errorWindow.setText("Synchronization failed ! See details.");
                errorWindow.exec();
                genSubtiles->setEnabled(true);
            }
        }
    }
    else if (modeSync->currentData().toInt() == 2) {

        genSubtiles->setEnabled(false);

        synchroEngine->tryMatchMulti();

        std::vector<Synchro::Match> matches = synchroEngine->getMatch();

        // -- check at least one match exists
        if (matches.size() <= 0) {
            QMessageBox errorWindow;
            errorWindow.setText("No match on duration ! No synchronization possible !");
            errorWindow.exec();
        }
        else {
            synchroEngine->fillMatches();
            synchroEngine->checkFilled();
            viewer->setData(synchroEngine, false);
            viewer->show();

            if (synchroEngine->isSynchronized()) {
                QMessageBox textWindow;
                textWindow.setText("Synchronization succeed ! Start delay : " + 
                QString::number(synchroEngine->getStartDelay()) + ", EntryPoint : " + 
                QString::number(synchroEngine->getEntryPoint()));
                textWindow.exec();

                if (folder.isEmpty()) {
                    QMessageBox errorWindow;
                    errorWindow.setText("Output directory not specified !");
                    errorWindow.exec();
                }
                else {
                    std::string file = folder.toUtf8().constData();
                    file += "/";
                    file += cplRef->getCplId();
                    file += ".sync";
                    std::cout << "Output file " << file << std::endl;
                    SyncFile* syncFile = synchroEngine->generateSyncFile(file);
                    syncFile->save();
                    delete syncFile;
                }
            }
            else {
                QMessageBox errorWindow;
                errorWindow.setText("Synchronization failed ! See details.");
                errorWindow.exec();
            }
        }
    }
}