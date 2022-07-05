#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QBoxLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <sstream>

#include "Poco/Logger.h"
#include "Poco/ConsoleChannel.h"
#include "Converter.h"

#include "Poco/File.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/NamedNodeMap.h"

#include "Poco/StreamCopier.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPCookie.h"

#pragma once

class SYNC : public QWidget
{
    Q_OBJECT

public:
    explicit SYNC(QWidget *parent = nullptr);
    ~SYNC();

private slots:

    void chooseDir();
    void search();

private:

    void loadServersList();
    void parseResponse(std::string response);

    std::vector<std::string> ipServers;
    std::map<std::string, int*> cplsUuid;
    std::string folder;

    QPushButton* chooseDirectoryOut;
    QLineEdit* outDirectory;
    QPushButton* launchCommand;
    QLineEdit* searchPattern;
};
