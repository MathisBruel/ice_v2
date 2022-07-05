#include <cstdlib>
#include <iostream>
#include "Poco/Timestamp.h"

#pragma once

class ImsRequest
{

public:

    enum Protocol {
        HTTP,
        HTTPS,
        UDP
    };

    ImsRequest(std::string method, std::string body, std::string path, std::string contentType);
    ImsRequest(uint8_t* payload, uint32_t length);

    ~ImsRequest();

    // -- get type
    Protocol getProtocol() {return protocol;}

    // -- get HTTP
    std::string getMethod() {return method;}
    std::string getBody() {return body;}
    std::string getPath() {return path;}
    std::string getContentType() {return contentType;}

    // -- get UDP
    uint8_t* getPayload() {return payload;}
    uint32_t getLength() {return length;}

    // -- timestamp
    void updateTimestamp() {this->timestamp = Poco::Timestamp().epochMicroseconds();}
    uint64_t getTimestamp() {return timestamp;}

private:

    Protocol protocol;

    // for HTTP
    std::string method;
    std::string body;
    std::string path;
    std::string contentType;

    // for UDP
    uint8_t* payload;
    uint32_t length;

    uint64_t timestamp;
};