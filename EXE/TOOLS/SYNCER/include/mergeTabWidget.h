#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>
#include <QTableWidget>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QDebug>

#include "cisMerger.h"
#include "Poco/UUIDGenerator.h"

#include "viewer/cisViewer.h"

#pragma once

class MergeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MergeWidget(QWidget *parent = nullptr);
    ~MergeWidget();

private slots:

    void openFile();
    void handleSelection();
    void removeFile();
    void outputFile();
    void generate();
    void viewImages(int, int);

private:

    QPushButton* addCis;
    QPushButton* removeCis;
    QPushButton* outputCis;

    QLineEdit* nameOfMergeCis;
    QPushButton* generateCis;

    QTableWidget* listCIS;

    int width;
    int height;
    int bpp;
    TimeCodeType frameRate;
    int versionCis;
    bool lutToApplied;
};
