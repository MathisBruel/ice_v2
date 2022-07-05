#include <cstdlib>
#include <iostream>
#include <sstream>

#pragma once
#include "Poco/StreamCopier.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/Context.h"
#include "Poco/Net/HTTPBasicCredentials.h"
#include "Poco/Net/SSLException.h"
#include "Poco/Logger.h"
#include "Poco/Timestamp.h"
#include "Poco/Timespan.h"
#include "portable_timer.h"

#include "ImsMessageHandler.h"
#include "device/BarcoHandler.h"

class ImsCommunicator
{

public:

    ImsCommunicator(std::string host, uint16_t port, ImsRequest::Protocol protocol);
    ~ImsCommunicator();

    bool open();
    void close();

    // -- permit to parse the response for HTTP or UDP
    void addMessageHandler(ImsMessageHandler* handler);
    ImsMessageHandler* getHandler() {return handler;}
    ImsRequest::Protocol getProtocol() {return protocol;}

    bool tryLock() {return sessionMutex.tryLock();}
    void unlock() {return sessionMutex.unlock();}

    // -- equivalent for HTTP or UDP
    void sendRequest(std::shared_ptr<ImsRequest> request, std::shared_ptr<ImsResponse> response);

private:

    // -- base to connect
    std::string host;
    uint16_t port;
    Poco::Net::SocketAddress* address;
    ImsRequest::Protocol protocol;

    // -- message handler
    ImsMessageHandler* handler;
    Poco::Mutex sessionMutex;

    // -- connector
    Poco::Net::HTTPClientSession* session;
    Poco::Net::HTTPCookie* cookie;
    Poco::Net::StreamSocket* socketTCP;

    struct tcp_keepalive {
        u_long  onoff;
        u_long  keepalivetime;
        u_long  keepaliveinterval;
    };
};