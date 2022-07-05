#include <cstdlib>
#include <iostream>
#include "Poco/Format.h"

#pragma once
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/AutoPtr.h"
#include "ImsMessageHandler.h"
#include "Converter.h"


class DoremiHandler : public ImsMessageHandler
{

public:

    DoremiHandler();
    ~DoremiHandler();

    // -- for HTTP only
    ImsRequest* createLoginRequest(std::string username, std::string password);
    ImsRequest* createLogoutRequest();

    // -- control player
    ImsRequest* createPlayRequest();
    ImsRequest* createPauseRequest();
    ImsRequest* createResumeRequest();
    ImsRequest* createStopRequest();
    ImsRequest* createJumpTimeCode(std::string timecode);
    ImsRequest* createGetPlayerStatusRequest();

    // -- cpls
    ImsRequest* createGetListCplsRequest();
    ImsRequest* createGetCplInfoRequest(std::string cplId);
    ImsRequest* createGetCplContentRequest(std::string cplId);

    void handleResponse(std::string xmlString, std::shared_ptr<ImsResponse> response);

    void parseLogin(Poco::XML::Node* nodeLogin, ImsResponse* response);
    void parseFault(Poco::XML::Node* nodeFault, ImsResponse* response);
    void parseShowStatus(Poco::XML::Node* nodeStatus, ImsResponse* response);
    void parseListCpl(Poco::XML::Node* nodeCplListResponse, ImsResponse* response);
    void parseCplInfos(Poco::XML::Node* nodeCplInfoResponse, ImsResponse* response);
    void parseCplContent(Poco::XML::Node* nodeCplContentResponse, ImsResponse* response);

private : 

    // -- paths for requestiong
    inline static const std::string pathAuthenticate = "/dc/dcp/ws/v1/SessionManagement";
    inline static const std::string pathPlayer = "/dc/dcp/ws/v1/ShowControl";
    inline static const std::string pathCpl = "/dc/dcp/ws/v1/CPLManagement";

    // -- envelope type
    inline static const std::string envelope = "<Envelope><Body><ns0:%[0]s xmlns:ns0='http://www.doremilabs.com/dc/dcp/ws/v1_0'>%[1]s</ns0:%[0]s></Body></Envelope>";
    
    // -- params type for requesting
    inline static const std::string loginParams = "<username>%s</username><password>%s</password>";
    inline static const std::string sessionParams = "<sessionId>%s</sessionId>";
    inline static const std::string cplParams = "<sessionId>%s</sessionId><cplId>%s</cplId>";

    inline static const std::string contentType = "text/xml; charset=utf-8";

    std::string sessionId;

};