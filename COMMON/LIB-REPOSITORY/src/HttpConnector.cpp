#include "HttpConnector.h"

HttpConnector::HttpConnector(std::string host, uint16_t port, std::string username, std::string password)
    : CentralRepository(host, port, username, password)
{
    type = HTTP;
    httpClientCentral = nullptr;
    httpClientDownload = nullptr;
}
HttpConnector::~HttpConnector()
{
    if (httpClientCentral != nullptr) {
        httpClientCentral->socket().close();
        delete httpClientCentral;
        httpClientCentral = nullptr;
    }
    if (httpClientDownload != nullptr) {
        httpClientDownload->socket().close();
        delete httpClientDownload;
        httpClientDownload = nullptr;
    }
}

void HttpConnector::setDownloadInfo(std::string host, uint16_t port, std::string username, std::string password)
{
    this->hostDownload = host;
    this->portDownload = port;
    this->usernameDownload = username;
    this->passwordDownload = password;
}

bool HttpConnector::open()
{
    httpClientCentral = new Poco::Net::HTTPClientSession(host, port);
    httpClientCentral->setKeepAlive(true);

    httpClientDownload = new Poco::Net::HTTPClientSession(hostDownload, portDownload);
    httpClientDownload->setKeepAlive(true);

    // -- login central
    Poco::Net::HTTPRequest httpRequest("POST", "/login");
    httpRequest.setContentType("application/xml");
    std::string body = "<command><username>" + username + "</username><password>" + password + "</password></command>";
    httpRequest.setContentLength(body.length());

    try {
        // -- send request and set timestamp
        httpClientCentral->sendRequest(httpRequest) << body << std::flush;
    } catch (std::exception &e) {
        Poco::Logger::get("HttpConnector").error("No possible to send http request", __FILE__, __LINE__);
    }

    Poco::Net::HTTPResponse responseHttp;
    idSessionCentral = "";
    try {
        std::istream& stream = httpClientCentral->receiveResponse(responseHttp);
        std::vector<Poco::Net::HTTPCookie> cookies;
        responseHttp.getCookies(cookies);
        std::vector<Poco::Net::HTTPCookie>::iterator cookieIt = cookies.begin();
        for (; cookieIt != cookies.end(); cookieIt++) {

            if (cookieIt->getName() == "sessionId") {
                idSessionCentral = cookieIt->getValue();
            }
        }
    } catch (std::exception &e) {
        Poco::Logger::get("HttpConnector").error("No response received", __FILE__, __LINE__);
    }

    // -- login download
    idSessionDownload = "";
    body = "<command><username>" + usernameDownload + "</username><password>" + passwordDownload + "</password></command>";
    httpRequest.setContentLength(body.length());

    try {
        // -- send request and set timestamp
        httpClientCentral->sendRequest(httpRequest) << body << std::flush;
    } catch (std::exception &e) {
        Poco::Logger::get("HttpConnector").error("No possible to send http request", __FILE__, __LINE__);
    }

    try {
        std::istream& stream = httpClientCentral->receiveResponse(responseHttp);
        std::vector<Poco::Net::HTTPCookie> cookies;
        responseHttp.getCookies(cookies);
        std::vector<Poco::Net::HTTPCookie>::iterator cookieIt = cookies.begin();
        for (; cookieIt != cookies.end(); cookieIt++) {

            if (cookieIt->getName() == "sessionId") {
                idSessionDownload = cookieIt->getValue();
            }
        }
    } catch (std::exception &e) {
        Poco::Logger::get("HttpConnector").error("No response received", __FILE__, __LINE__);
    }

    return (!idSessionCentral.empty() && !idSessionDownload.empty());
}
void HttpConnector::close()
{
    if (httpClientCentral != nullptr) {
        httpClientCentral->socket().close();
        delete httpClientCentral;
        httpClientCentral = nullptr;
    }
    if (httpClientDownload != nullptr) {
        httpClientDownload->socket().close();
        delete httpClientDownload;
        httpClientDownload = nullptr;
    }
}

bool HttpConnector::checkDownloadScripts(std::string cinema, bool sas)
{
    if (idSessionCentral.empty()) {
        return false;
    }

    // -- send request
    Poco::Net::HTTPRequest httpRequest("POST", "/");
    httpRequest.setContentType("application/xml");
    std::string body = "<command><type>SET_COLOR</type><parameters><cinema>" + cinema + "</username><type>" + (sas ? "true" : "false") + "</type></parameters></command>";
    httpRequest.setContentLength(body.length());

    Poco::Net::NameValueCollection cookies;
    cookies.add("sessionId", idSessionCentral);
    httpRequest.setCookies(cookies);

    try {
        // -- send request and set timestamp
        httpClientCentral->sendRequest(httpRequest) << body << std::flush;
    } catch (std::exception &e) {
        Poco::Logger::get("HttpConnector").error("No possible to send http request", __FILE__, __LINE__);
        return false;
    }

    // -- receive response
    Poco::Net::HTTPResponse responseHttp;
    std::stringstream stringStream;
    try {
        std::istream& stream = httpClientCentral->receiveResponse(responseHttp);
        Poco::StreamCopier::copyStream(stream, stringStream);
        
    } catch (std::exception &e) {
        Poco::Logger::get("HttpConnector").error("No response received", __FILE__, __LINE__);
        return false;
    }

    // -- parse response
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    
    try {
        doc = parser.parseString(stringStream.str());
    } catch (std::exception &e) {
        Poco::Logger::get("Command").error("Exception when parsing XML file !", __FILE__, __LINE__);
        Poco::Logger::get("Command").debug("Content : " + stringStream.str(), __FILE__, __LINE__);
        return false;
    }

    

    return true;
}
bool HttpConnector::downloadScript(std::shared_ptr<ScriptInfoFile> scriptPath)
{
    return true;
}