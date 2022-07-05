#include "CentralRepository.h"

class FtpConnector : public CentralRepository
{

public:

    FtpConnector(std::string host, uint16_t port, std::string username, std::string password);
    ~FtpConnector();

    bool open();
    void close();
    bool openDownload();
    void closeDownload();
    bool checkAvailableScripts();
    bool downloadFile(std::string ftpPath, std::string savePath);
    void addPathToCheck(std::string path);

    inline static const std::string pathFtpScriptFilm = "/CINEMAS/ICE/SCRIPT-VIBES-FILM";
    inline static const std::string pathFtpScript = "/CINEMAS/ICE/SCRIPT-VIBES";
    inline static const std::string pathFtpScriptAmbiance = "/CINEMAS/ICE/SCRIPT-VIBES-AMBIANCE";
    inline static const std::string pathFtpScriptHisto = "/CINEMAS/ICE/SCRIPT-VIBES-HISTO";
    inline static const std::string pathFtpScriptTest = "/CINEMAS/ICE/SCRIPT-VIBES-TEST";

private:

    bool listInDirectory(std::string path);

    Poco::Net::FTPClientSession* ftpClient;
    Poco::Net::FTPClientSession* ftpDownload;
    std::vector<std::string> pathToCheck;

};