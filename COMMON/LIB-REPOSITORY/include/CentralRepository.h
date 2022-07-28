#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <fstream>

#pragma once
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/StreamCopier.h"
#include "Poco/Net/FTPClientSession.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Logger.h"
#include "ScriptInfoFile.h"
#include "portable_timer.h"

class CentralRepository
{

public:

    enum ConnectorType {
        FTP, 
        HTTP,
        UNKNOWN
    };

    CentralRepository(std::string host, uint16_t port, std::string username, std::string password);
    ~CentralRepository();
    void setDownloadPath(std::string path) {this->downloadPath = path;}

    // -- generic function (available for HTTP or FTP)
    virtual bool open() {return false;}
    virtual void close() {}
    virtual bool openDownload() {return false;}
    virtual void closeDownload() {}
    virtual bool checkDownloadScripts(std::string cinema, bool sas) {return false;}
    virtual bool checkAvailableScripts() {return false;}
    virtual bool downloadFile(std::string ftpPath, std::string savePath) {return false;}

    // -- specific HTTP
    std::vector<std::shared_ptr<ScriptInfoFile>> getScriptsDownload() {return infosForDownloading;}

    ConnectorType getType() {return type;}

    void addDownload(std::string srcPath, std::string dstPath);
    std::map<std::string, std::string> getFilesToDownloadList() {return filesToDownload;}
    void removeDownload(std::string key);
    bool tryLockDownloadMutex() {return downloadMutex.tryLock();}
    void unlockDownloadMutex() {downloadMutex.unlock();}

protected:

    // -- define way to connect to central REPO
    std::string host;
    uint16_t port;
    std::string username;
    std::string password;

    // -- where to save files
    std::string downloadPath;

    // -- data download infos
    std::vector<std::shared_ptr<ScriptInfoFile>> infosForDownloading;

    std::map<std::string, std::string> filesToDownload;
    Poco::Mutex downloadMutex;

    // -- to differentiate connectors
    ConnectorType type;
};