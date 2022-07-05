#include "CentralRepository.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/NamedNodeMap.h"

class HttpConnector : public CentralRepository
{

public:

    HttpConnector(std::string host, uint16_t port, std::string username, std::string password);
    ~HttpConnector();

    void setDownloadInfo(std::string host, uint16_t port, std::string username, std::string password);

    bool open();
    void close();
    bool checkDownloadScripts(std::string cinema, bool sas);
    bool downloadScript(std::shared_ptr<ScriptInfoFile> scriptPath);

private:

    Poco::Net::HTTPClientSession* httpClientCentral;
    std::string idSessionCentral;
    Poco::Net::HTTPClientSession* httpClientDownload;
    std::string idSessionDownload;

    std::string hostDownload;
    uint16_t portDownload;
    std::string usernameDownload;
    std::string passwordDownload;
};