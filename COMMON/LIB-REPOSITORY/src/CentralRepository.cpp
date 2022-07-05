#include "CentralRepository.h"

CentralRepository::CentralRepository(std::string host, uint16_t port, std::string username, std::string password)
{
    this->host = host;
    this->port = port;
    this->username = username;
    this->password = password;

    downloadPath = "";

    type = UNKNOWN;
}
    
CentralRepository::~CentralRepository() {}

void CentralRepository::addDownload(std::string srcPath, std::string dstPath)
{
    while (!downloadMutex.tryLock()) {Timer::crossUsleep(20);}
    if (filesToDownload.find(srcPath) == filesToDownload.end()) {
        filesToDownload.insert_or_assign(srcPath, dstPath);
    }
    downloadMutex.unlock();
}

void CentralRepository::removeDownload(std::string key)
{
    while (!downloadMutex.tryLock()) {Timer::crossUsleep(20);}
    filesToDownload.erase(key);
    downloadMutex.unlock();
}