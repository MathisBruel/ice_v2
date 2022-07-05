#include "mergeTabWidget.h"

MergeWidget::MergeWidget(QWidget *parent) :
    QWidget(parent)
{
    // -- init attributes
    this->width = -1;
    this->height = -1;
    this->bpp = -1;
    this->frameRate = TimeCodeType::Invalid;
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // -- create buttons
    addCis = new QPushButton(QIcon("images/add.png"), "");
    addCis->setIconSize(QSize(50, 50));
    connect(addCis, SIGNAL(released()), this, SLOT(openFile()));
    removeCis = new QPushButton(QIcon("images/remove.png"), "");
    removeCis->setEnabled(false);
    removeCis->setIconSize(QSize(50, 50));
    connect(removeCis, SIGNAL(released()), this, SLOT(removeFile()));
    outputCis = new QPushButton(QIcon("images/save.png"), "");
    outputCis->setIconSize(QSize(50, 50));
    connect(outputCis, SIGNAL(released()), this, SLOT(outputFile()));

    // -- create widgets
    nameOfMergeCis = new QLineEdit("");
    generateCis = new QPushButton(QIcon("images/generate.png"), "");
    generateCis->setIconSize(QSize(50, 50));
    connect(generateCis, SIGNAL(released()), this, SLOT(generate()));

    // -- create list
    listCIS = new QTableWidget(0, 2, this);
    QStringList header;
    header.append("File");
    header.append("Nb frames");
    listCIS->setHorizontalHeaderLabels(header);
    listCIS->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listCIS->setSelectionBehavior(QAbstractItemView::SelectRows);
    listCIS->setSelectionMode(QAbstractItemView::SingleSelection);
    listCIS->setShowGrid(false);
    listCIS->setColumnWidth(0, 960);
    QHeaderView* headerView = listCIS->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Interactive);
    headerView->setStretchLastSection(true);
    connect(listCIS, SIGNAL(itemSelectionChanged()), this, SLOT(handleSelection()));
    connect(listCIS, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(viewImages(int, int)));
    QHeaderView* rowView = listCIS->verticalHeader();
    rowView->setSectionsMovable(true);

    // -- create layouts
    QHBoxLayout* generateLayout = new QHBoxLayout();
    generateLayout->addWidget(addCis);
    generateLayout->addWidget(removeCis);
    generateLayout->addWidget(outputCis);
    generateLayout->addSpacing(5);
    generateLayout->addWidget(nameOfMergeCis);
    generateLayout->addSpacing(5);
    generateLayout->addWidget(generateCis);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addItem(generateLayout);
    globalLayout->addSpacing(2);
    globalLayout->addWidget(listCIS);
    globalLayout->addStretch();

    setLayout(globalLayout);
}

MergeWidget::~MergeWidget()
{
    delete addCis;
    delete removeCis;

    delete nameOfMergeCis;
    delete generateCis;

    delete listCIS;
}

void MergeWidget::openFile()
{
    QString file = QFileDialog::getOpenFileName(this, "Choose CIS file to add", QString(), "*.cis");
    if (!file.isEmpty()) {
        qInfo() << "File selected is " << file;

        // -- try to find if existing entry
        int idxRow = listCIS->rowCount();
        bool found = false;
        for (int i = 0; i < listCIS->rowCount(); i++) {
            QTableWidgetItem* item = listCIS->item(i, 0);
            if (item->text() == file) {
                found = true;
                break;
            }
        }

        // -- error if already present
        if (found) {
            QMessageBox errorWindow;
            errorWindow.setText("Can not add an already existing cis file : " + file);
            errorWindow.exec();
            return;
        }

        // -- open cis file to get header values
        Cis cis;
        if (!cis.open(file.toUtf8().constData())) {
            QMessageBox errorWindow;
            errorWindow.setText("Error while opening : " + file);
            errorWindow.exec();
            return;
        }

        // -- init attributes for merge cis or check if valid if already added
        if (width != -1) {
            if (cis.getFrameWidth() != width) {
                QMessageBox errorWindow;
                errorWindow.setText("Cis width does not match current width !");
                errorWindow.exec();
                return;
            }
            if (cis.getFrameHeight() != height) {
                QMessageBox errorWindow;
                errorWindow.setText("Cis height does not match current height !");
                errorWindow.exec();
                return;
            }
            if (cis.getBitsPerPixel() != bpp) {
                QMessageBox errorWindow;
                errorWindow.setText("Cis bpp does not match current bpp !");
                errorWindow.exec();
                return;
            }
            if (cis.getFrameRate() != frameRate) {
                QMessageBox errorWindow;
                errorWindow.setText("Cis frameRate does not match current frameRate !");
                errorWindow.exec();
                return;
            }
            if (cis.getVersion() != versionCis) {
                QMessageBox errorWindow;
                errorWindow.setText("Cis version does not match current version !");
                errorWindow.exec();
                return;
            }
        }
        else {
            width = cis.getFrameWidth();
            height = cis.getFrameHeight();
            bpp = cis.getBitsPerPixel();
            frameRate = cis.getFrameRate();
            versionCis = cis.getVersion();
        }

        QString nbFrames = QString::number(cis.getFrameCount());

        // -- add line on list
        listCIS->insertRow(idxRow);
        listCIS->setItem(idxRow, 0, new QTableWidgetItem(file));
        listCIS->setItem(idxRow, 1, new QTableWidgetItem(nbFrames));

    }
}

void MergeWidget::handleSelection()
{
    QModelIndexList indexes = listCIS->selectionModel()->selectedRows();

    if (indexes.size() != 1) {
        qDebug() << "More than one file selected in list ! ";
        removeCis->setEnabled(false);
        return;
    }
    removeCis->setEnabled(true);
}

void MergeWidget::removeFile()
{
    QModelIndexList indexes = listCIS->selectionModel()->selectedRows();

    if (indexes.size() != 1) {
        return;
    }
    listCIS->removeRow(indexes.at(0).row());
    listCIS->selectionModel()->clearSelection();

    // -- reset parameters if no more cis file
    if (listCIS->rowCount() == 0) {
        this->width = -1;
        this->height = -1;
        this->bpp = -1;
        this->frameRate = TimeCodeType::Invalid;
    }
}

void MergeWidget::outputFile()
{
    QString fileSave = QFileDialog::getSaveFileName(this, tr("Choose file to save"), QString(), "*.cis");
    if (!fileSave.isEmpty()) {
        nameOfMergeCis->setText(fileSave);
    }
}

void MergeWidget::generate()
{
    if (listCIS->rowCount() <= 1) {
        QMessageBox errorWindow;
        errorWindow.setText("Can only merge cis if more than 1 cis file !");
        errorWindow.exec();
        return;
    }

    if (nameOfMergeCis->text().isEmpty()) {
        QMessageBox errorWindow;
        errorWindow.setText("Output file is not defined !");
        errorWindow.exec();
        return;
    }

    CisMerger merger;
    merger.setImageFormat(width, height, bpp, frameRate);
    std::string versionType = "";
    if (versionCis == 1) {versionType = "CIS-V001";}
    else if (versionCis == 2) {versionType = "CIS-V002";}

    for (int i = 0; i < listCIS->rowCount(); i++) {

        QTableWidgetItem* item = listCIS->item(i, 0);
        qDebug() << "Treat " << item->text();
        Cis* cis = new Cis();
        if (!cis->open(item->text().toUtf8().constData())) {
            QMessageBox errorWindow;
            errorWindow.setText("Error while opening : " + item->text());
            errorWindow.exec();
            return;
        }

        merger.addCis(cis);
    }

    if (merger.save(nameOfMergeCis->text().toUtf8().constData(), versionType)) {
        QMessageBox returnWindow;
        returnWindow.setText("Merge done !");
        returnWindow.exec();
    }
    else {
        QMessageBox errorWindow;
        errorWindow.setText("Error while merging !");
        errorWindow.exec();
    }
}

void MergeWidget::viewImages(int, int)
{
    QModelIndexList indexes = listCIS->selectionModel()->selectedRows();

    if (indexes.size() != 1) {
        return;
    }

    QTableWidgetItem* item = listCIS->item(indexes.at(0).row(), 0);
    CisViewer* viewer = new CisViewer(item->text());
    viewer->show();
}
