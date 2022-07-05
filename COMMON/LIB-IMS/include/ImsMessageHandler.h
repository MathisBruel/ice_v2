#include <cstdlib>
#include <iostream>

#pragma once

#include "Poco/Net/HTTPRequest.h"

#include "ImsResponse.h"
#include "xml/PlayerStatus.h"
#include "xml/CplInfos.h"
#include "Poco/Logger.h"

class ImsMessageHandler
{

public: 

    ImsMessageHandler();
    ~ImsMessageHandler();

    // -- for HTTP only
    virtual ImsRequest* createLoginRequest(std::string username, std::string password) {return nullptr;}
    virtual ImsRequest* createLogoutRequest() {return nullptr;}

    // -- control player
    virtual ImsRequest* createPlayRequest() {return nullptr;}
    virtual ImsRequest* createPauseRequest() {return nullptr;}
    virtual ImsRequest* createResumeRequest() {return nullptr;}
    virtual ImsRequest* createStopRequest() {return nullptr;}
    virtual ImsRequest* createJumpTimeCode(std::string timecode) {return nullptr;}
    virtual ImsRequest* createGetPlayerStatusRequest() {return nullptr;}

    // -- specific CHRISTIE
    virtual ImsRequest* createGetStatusItemRequest(uint32_t statusItemCode) {return nullptr;}

    // -- cpls
    virtual ImsRequest* createGetListCplsRequest() {return nullptr;}
    virtual ImsRequest* createGetCplInfoRequest(std::string cplId) {return nullptr;}
    virtual ImsRequest* createGetCplContentRequest(std::string cplId) {return nullptr;}

    virtual void handleResponse(std::string xmlString, std::shared_ptr<ImsResponse> response) {}
    virtual void handleResponse(uint8_t* buffer, uint16_t size, std::shared_ptr<ImsResponse> response) {}

    bool isLoggedIn() {return loggedIn;}

protected:

    bool loggedIn;
};