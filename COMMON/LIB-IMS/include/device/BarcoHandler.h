#include <cstdlib>
#include <iostream>
#include "Poco/Format.h"

#pragma once
#include "Poco/JSON/Parser.h"
#include "ImsMessageHandler.h"
#include "Converter.h"


class BarcoHandler : public ImsMessageHandler
{

public:

    BarcoHandler();
    ~BarcoHandler();

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
    ImsRequest* createGetCplInfoRequest(std::string cplId) {return nullptr;}
    ImsRequest* createGetCplContentRequest(std::string cplId);

    void handleResponse(std::string jsonString, std::shared_ptr<ImsResponse> response);

    std::string getSessionId() {return sessionId;}

private : 

    void parseLogin(Poco::JSON::Object::Ptr loginParams, ImsResponse* response);
    void parseShowStatus(Poco::JSON::Object::Ptr nodeStatus, ImsResponse* response);
    void parseCplInfos(Poco::JSON::Object::Ptr nodeCplInfoResponse, ImsResponse* response);
    void parseCplContent(Poco::JSON::Object::Ptr nodeCplContentResponse, ImsResponse* response);

    // -- path for requesting
    inline static const std::string pathAuthenticate = "/smscontrol?reqtype=sessioncommand";
    inline static const std::string pathPlayer = "/smscontrol?reqtype=smscommand";

    // -- envelope type
    inline static const std::string envelope = "{\"command\": {\"name\": \"%s\",\"parameters\": {%s}}}";
    //inline static const std::string envelope = "<Envelope xmlns='http://schemas.xmlsoap.org/soap/envelope/'><Body><%[0]s xmlns='http://www.barco.com/sms/sms_1'>%[1]s</%[0]s></Body></Envelope>";

    // -- params type for requesting
    inline static const std::string loginParams = "\"username\": \"%s\",\"password\": \"%s\",\"location\": \"\"";
    //inline static const std::string loginParams = "<userName>\"%s\"</userName><password>\"%s\"</password><sessionInfo></sessionInfo>";
    inline static const std::string playerStatusParams = "\"sessionid\": \"%s\"";
    inline static const std::string cplParams = "\"id\":\"%s\"";
    inline static const std::string jumpParams = "\"newPosition\":\"%s\"";

    inline static const std::string contentType = "application/json";
    //inline static const std::string contentType = "text/xml; charset=utf-8";

    double currentEditRate;
    std::string sessionId;
};