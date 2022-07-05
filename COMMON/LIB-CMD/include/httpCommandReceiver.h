#include <cstdlib>
#include <iostream>
#include <sstream>

#pragma once

#include "Poco/Logger.h"
#include "Poco/Mutex.h"
#include "Poco/Net/HTTPCookie.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/MediaType.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/NamedNodeMap.h"

#include "session.h"
#include "command.h"
#include "commandResponse.h"
#include "portable_timer.h"

class HttpCommandHandler : public Poco::Net::HTTPRequestHandler
{

public:

    // -- session
    static std::map<std::string, Session*> httpSessions;
    // -- commands & responses

    static std::map<std::string, std::shared_ptr<Command>> commands;
    static std::map<std::string, std::shared_ptr<CommandResponse>> responses;
    static Poco::Mutex commandsMutex;
    static Poco::Mutex responsesMutex;

    virtual void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp);

    std::shared_ptr<Command> getFirstCommand();
    std::shared_ptr<CommandResponse> getResponse(std::string uuid);
    void deleteCommand(std::string uuid);
    void deleteResponse(std::string uuid);
    void addResponse(std::shared_ptr<CommandResponse> response);
    void addCommand(std::shared_ptr<Command> command);

    void removeOldResponses();

private :

    // -- handling requests
    bool parseLoginRequest(std::istream& content);
    std::shared_ptr<CommandResponse> parseCommandRequest(std::istream& content);
};

class HttpCommandHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    HttpCommandHandlerFactory() {}

    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request)
    {
        return new HttpCommandHandler();
    }
};