#include "FtpConnector.h"

FtpConnector::FtpConnector(std::string host, uint16_t port, std::string username, std::string password) 
    : CentralRepository(host, port, username, password)
{
    type = FTP;
    ftpClient = nullptr;
    ftpDownload = nullptr;
}

FtpConnector::~FtpConnector()
{
    if (ftpClient != nullptr) {
        ftpClient->logout();
        ftpClient->close();
        delete ftpClient;
        ftpClient = nullptr;
    }

    if (ftpDownload != nullptr) {
        ftpDownload->logout();
        ftpDownload->close();
        delete ftpDownload;
        ftpDownload = nullptr;
    }
}

void FtpConnector::close()
{
    if (ftpClient != nullptr) {
        ftpClient->logout();
        ftpClient->close();
        delete ftpClient;
        ftpClient = nullptr;
    }
}
bool FtpConnector::open()
{
    try {
        ftpClient = new Poco::Net::FTPClientSession(host, port, username, password);
        ftpClient->setTimeout(Poco::Timespan(30, 0));
    } catch (std::exception &e) {
        Poco::Logger::get("FtpConnector").error("Error while opening FTP server", __FILE__, __LINE__);
        return false;
    }
    
    if (!ftpClient->isOpen()) {
        Poco::Logger::get("FtpConnector").error("Socket to FTP server is not open !", __FILE__, __LINE__);
        return false;
    }
    return true;
}

bool FtpConnector::openDownload()
{
    try {
        ftpDownload = new Poco::Net::FTPClientSession(host, port, username, password);
        ftpDownload->setTimeout(Poco::Timespan(30, 0));
    } catch (std::exception &e) {
        Poco::Logger::get("FtpConnector").error("Error while opening FTP server", __FILE__, __LINE__);
        return false;
    }
    
    if (!ftpDownload->isOpen()) {
        Poco::Logger::get("FtpConnector").error("Socket to FTP server is not open !", __FILE__, __LINE__);
        return false;
    }
    return true;
}
void FtpConnector::closeDownload()
{
    if (ftpDownload != nullptr) {
        ftpDownload->logout();
        ftpDownload->close();
        delete ftpDownload;
        ftpDownload = nullptr;
    }
}

bool FtpConnector::checkAvailableScripts()
{
    infosForDownloading.clear();
    for (int i = 0; i < pathToCheck.size(); i++) {
        Poco::Logger::get("FtpConnector").information("Checking FTP path : " + pathToCheck.at(i), __FILE__, __LINE__);
        if (!listInDirectory(pathToCheck.at(i))) {
            return false;
        }
    }
    return true;
}

void FtpConnector::addPathToCheck(std::string path)
{
    bool found = false;
    for (int i = 0; i < pathToCheck.size(); i++) {    
        if (pathToCheck.at(i) == path) {
            found = true;
            break;
        }
    }

    if (!found) {
        pathToCheck.push_back(path);
    }
}
    
bool FtpConnector::listInDirectory(std::string path)
{
    // -- we check that connexion is opened and session is logged in
    if (!ftpClient->isOpen() || !ftpClient->isLoggedIn()) {
        Poco::Logger::get("FtpConnector").error("Socket to FTP is not logged in or open !", __FILE__, __LINE__);
        return false;
    }

    // -- first we list all directory of scripts
    std::vector<std::string> pathToScripts;
    std::string list;
    try {
        std::istream &stream = ftpClient->beginList(path, false);
        stream >> list;
        while (!stream.eof()) {
            pathToScripts.push_back(list);
            stream >> list;
        }
        ftpClient->endList();

    } catch (std::exception &e) {
        Poco::Logger::get("FtpConnector").error("Exception happened while listing directory scripts in " + path, __FILE__, __LINE__);
        Poco::Logger::get("FtpConnector").error(e.what(), __FILE__, __LINE__);
        return false;
    }

    // -- then check if users file exists
    for (int i = 0; i < pathToScripts.size(); i++) {

        Poco::Path pathFileToScripts(pathToScripts.at(i));
        std::shared_ptr<ScriptInfoFile> info = std::make_shared<ScriptInfoFile>(ScriptInfoFile());
        info->setNameScript(pathFileToScripts.getFileName());
        info->setFtpPath(path);
        Poco::Logger::get("FtpConnector").information("Checking script : " + info->getNameScript(), __FILE__, __LINE__);
        bool authorized = false;
        try {
            std::istream &stream = ftpClient->beginList(pathToScripts.at(i), true);
            while (!stream.eof()) {

                // -- with extended option we get the size and name of file 
                // -- rights / unused / unused / unused / size / month / day / year / filename
                std::string fileName = "";
                long size;
                for (int i = 0; i < 9; i++) {

                    if (stream.eof()) {
                        break;
                    }
                    stream >> list;
                    if (i == 4) {
                        size = std::stol(list);;
                    }
                    if (i == 8) {
                        fileName = list;
                    }
                }

                if (fileName != "") {

                    // -- we keep folder
                    if (fileName == "users") {authorized = true;}
                    else {
                        Poco::Path file(fileName);
                        if (file.getExtension() == "cis") 
                        {
                            info->setCisInfo(file.getBaseName(), size);
                            Poco::Logger::get("FtpConnector").information("Cis file : " + file.getBaseName() + ".cis size " + std::to_string(size), __FILE__, __LINE__);
                        }
                        else if (file.getExtension() == "lvi") 
                        {
                            info->setLviInfo("", size);
                            Poco::Logger::get("FtpConnector").information("Lvi file : " + info->getSha1Cis() + ".lvi size " + std::to_string(size), __FILE__, __LINE__);
                        }
                        else if (file.getExtension() == "sync") 
                        {
                            info->addSync(file.getBaseName(), "", size);
                            Poco::Logger::get("FtpConnector").information("Sync file : " + file.getBaseName() + ".sync size " + std::to_string(size), __FILE__, __LINE__);
                        }
                    }
                }

            }
            ftpClient->endList();

        } catch (std::exception &e) {
            Poco::Logger::get("FtpConnector").error("Exception happened while listing scripts files in " + pathToScripts.at(i), __FILE__, __LINE__);
            Poco::Logger::get("FtpConnector").error(e.what(), __FILE__, __LINE__);
        }

        // -- we finally add it if necessary
        if (authorized) {
            Poco::Logger::get("FtpConnector").information("Saving script infos", __FILE__, __LINE__);
            infosForDownloading.push_back(info);
        }
    }
    return true;
}

bool FtpConnector::downloadFile(std::string ftpPath, std::string savePath)
{
    // -- we check that connexion is opened and session is logged in
    if (!ftpDownload->isOpen() || !ftpDownload->isLoggedIn()) {
        Poco::Logger::get("FtpConnector").error("Socket to FTP is not logged in or open !", __FILE__, __LINE__);
        return false;
    }

    // -- finally we download files of script directory
    try {
        // -- we determine and create stream to file to be saved at
        std::ofstream fileStream;
        fileStream.open(savePath, std::ios::out | std::ios::trunc | std::ios::binary);

        // -- we download
        std::istream &stream = ftpDownload->beginDownload(ftpPath);
        Poco::StreamCopier::copyStream(stream, fileStream);
        ftpDownload->endDownload();
        fileStream.close();

    } catch (std::exception &e) {
        Poco::Logger::get("FtpConnector").error("Exception happened while download of scripts files " + ftpPath, __FILE__, __LINE__);
    }

    return true;
}