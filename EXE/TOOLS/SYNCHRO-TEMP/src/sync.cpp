#include "sync.h"

SYNC::SYNC(QWidget *parent) :
    QWidget(parent)
{
    Poco::Logger::root().setChannel(new Poco::ConsoleChannel());
    Poco::Logger::root().setLevel(Poco::Message::PRIO_DEBUG);

    chooseDirectoryOut = new QPushButton("Directory");
    connect(chooseDirectoryOut, SIGNAL(released()), this, SLOT(chooseDir()));

    outDirectory = new QLineEdit("");
    outDirectory->setReadOnly(true);
    searchPattern = new QLineEdit("");

    launchCommand = new QPushButton("Search");
    connect(launchCommand, SIGNAL(released()), this, SLOT(search()));

    QHBoxLayout* line = new QHBoxLayout();
    line->addWidget(chooseDirectoryOut);
    line->addWidget(outDirectory);
    line->addWidget(searchPattern);

    QVBoxLayout* globalLayout = new QVBoxLayout();
    globalLayout->addItem(line);
    globalLayout->addSpacing(2);
    globalLayout->addWidget(launchCommand);
    globalLayout->addStretch();
    setLayout(globalLayout);

    loadServersList();
}

SYNC::~SYNC() {}

void SYNC::chooseDir()
{
    QString folderTemp = QFileDialog::getExistingDirectory(this, tr("Choose output directory"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!folderTemp.isEmpty()) {
        folder = folderTemp.toStdString();
        outDirectory->setText(folderTemp + "/");
    }
}

void SYNC::search()
{
    if (searchPattern->text().isEmpty()) {
        QMessageBox errorWindow;
        errorWindow.setText("Search pattern can't be empty !");
        errorWindow.exec();
        return;
    }

    if (folder.empty()) {
        QMessageBox errorWindow;
        errorWindow.setText("Output folder can't be empty !");
        errorWindow.exec();
        return;
    }

    // -- for each server
    for (int i = 0; i < ipServers.size(); i++) {

        // -- connect to server API
        Poco::Net::HTTPClientSession* sessionICE = new Poco::Net::HTTPClientSession(ipServers.at(i), 8300);
        sessionICE->setTimeout(Poco::Timespan(10, 0));

        // -- login to API
        Poco::Net::HTTPRequest httpRequest("POST", "/login");
        httpRequest.setContentType("application/xml");
        std::string bodyLogin = "<command><username>admin</username><password>ice17180</password></command>";
        httpRequest.setContentLength(bodyLogin.length());
        try {
            // -- send request and set timestamp
            sessionICE->sendRequest(httpRequest) << bodyLogin << std::flush;
        } catch (std::exception &e) {
            Poco::Logger::get("Render").error("No possible to send http request", __FILE__, __LINE__);
        }

        // -- retrieve sessionID
        Poco::Net::HTTPResponse responseHttp;
        std::string sessionId = "";
        try {
            std::istream& stream = sessionICE->receiveResponse(responseHttp);
            std::vector<Poco::Net::HTTPCookie> cookies;
            responseHttp.getCookies(cookies);
            std::vector<Poco::Net::HTTPCookie>::iterator cookieIt = cookies.begin();
            for (; cookieIt != cookies.end(); cookieIt++) {

                if (cookieIt->getName() == "sessionId") {
                    sessionId = cookieIt->getValue();
                }
                Poco::Logger::get("Render").debug("Cookie key: " + cookieIt->getName() + " !", __FILE__, __LINE__);
                Poco::Logger::get("Render").debug("Cookie value: " + cookieIt->getValue() + " !", __FILE__, __LINE__);
            }
        } catch (std::exception &e) {
            Poco::Logger::get("Render").error("No response received", __FILE__, __LINE__);
        }

        // -- do not continue if not identified
        if (sessionId.empty()) {
            delete sessionICE;
            continue;
        }

        // -- ask for CPLs to API
        Poco::Net::HTTPRequest httpRequestCpl("POST", "/");
        httpRequestCpl.setContentType("application/xml");
        std::string body = "<command><type>GET_CPL_CONTENT_NAME</type><parameters><search>" + searchPattern->text().toStdString() + "</search></parameters></command>";
        httpRequestCpl.setContentLength(body.length());
        Poco::Net::NameValueCollection cookies;
        cookies.add("sessionId", sessionId);
        httpRequestCpl.setCookies(cookies);

        try {
            // -- send request and set timestamp
            sessionICE->sendRequest(httpRequestCpl) << body << std::flush;
        } catch (std::exception &e) {
            Poco::Logger::get("Render").error("No possible to send http request", __FILE__, __LINE__);
        }

        Poco::Net::HTTPResponse responseHttpCpl;
        std::stringstream stringStream;
        try {
            std::istream& stream = sessionICE->receiveResponse(responseHttpCpl);
            Poco::StreamCopier::copyStream(stream, stringStream);

        } catch (std::exception &e) {
            Poco::Logger::get("Render").error("No response received", __FILE__, __LINE__);
        }

        // -- close session
        delete sessionICE;

        // -- exit extraction if empty
        if (stringStream.str().empty()) {
            continue;
        }

        parseResponse(stringStream.str());
    }
}

void SYNC::loadServersList()
{
    // -- check file exists
    Poco::File fileServers("servers.xml");
    if (!fileServers.exists()) {
        Poco::Logger::get("SYNC").error("File servers.xml do not exists !", __FILE__, __LINE__);
        return;
    }

    // -- load file
    std::ifstream in("servers.xml");
    Poco::XML::InputSource src(in);

    // -- parse
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;

    try {
        doc = parser.parse(&src);
    } catch (std::exception &e) {
        Poco::Logger::get("SYNC").error("Exception when parsing XML CPL !", __FILE__, __LINE__);
        in.close();
        return;
    }
    in.close();

    Poco::XML::NodeList* listServers = doc->childNodes();
    for (int i = 0; i < listServers->length(); i++) {

        if (listServers->item(i)->nodeName().find("Servers") != -1) {
            Poco::XML::NodeList* listServer = listServers->item(i)->childNodes();

            for (int j = 0; j < listServer->length(); j++) {
                if (listServer->item(j)->nodeName().find("Server") != -1) {
                    ipServers.push_back(listServer->item(j)->innerText());
                    Poco::Logger::get("SYNC").debug(listServer->item(j)->innerText(), __FILE__, __LINE__);
                }
            }

            listServer->release();
        }
    }
    listServers->release();
    doc->release();
}

void SYNC::parseResponse(std::string response)
{
    // -- parse
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;

    try {
        doc = parser.parseString(response);
    } catch (std::exception &e) {
        Poco::Logger::get("SYNC").error("Exception when parsing XML CPL !", __FILE__, __LINE__);
        return;
    }

    Poco::XML::NodeList* cpls = doc->childNodes();
    for (int i = 0; i < cpls->length(); i++) {

        // -- found CPLS tag
        if (cpls->item(i)->nodeName().find("CPLS") != -1) {

            Poco::XML::NodeList* cpl = cpls->item(i)->childNodes();
            for (int j = 0; j < cpl->length(); j++) {

                // -- for each CPL element
                if (cpl->item(j)->nodeName().find("CPL") != -1) {

                    std::string uuid;
                    std::string content;

                    // -- get attribute uuid
                    Poco::XML::NamedNodeMap* attributes = cpl->item(j)->attributes();
                    for (int a = 0; a < attributes->length(); a++) {
                        if (attributes->item(a)->nodeName() == "") {
                            uuid = attributes->item(a)->innerText();
                        }
                    }
                    attributes->release();

                    // -- get content of CPL
                    Poco::XML::NodeList* contents = cpl->item(j)->childNodes();
                    for (int c = 0; c < contents->length(); c++) {
                        if (contents->item(c)->nodeName() == "Content") {
                            content = Converter::decodeBase64(contents->item(c)->innerText());
                        }
                    }
                    contents->release();

                    // -- save file
                    std::string filename = folder + "/" + uuid + ".xml";
                    std::ofstream fileStream;
                    fileStream.open(filename);
                    fileStream << content;
                    fileStream.close();
                }
            }
            cpl->release();
        }
    }

    cpls->release();
    doc->release();
}