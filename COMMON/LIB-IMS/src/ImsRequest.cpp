#include "ImsRequest.h"

ImsRequest::ImsRequest(std::string method, std::string body, std::string path, std::string contentType)
{
    this->method = method;
    this->body = body;
    this->path = path;
    this->contentType = contentType;
    this->payload = nullptr;
    protocol = Protocol::HTTP;
}

ImsRequest::ImsRequest(uint8_t* payload, uint32_t length)
{
    this->payload = payload;
    this->length = length;
    protocol = Protocol::UDP;
}
    
ImsRequest::~ImsRequest()
{
    // -- payload delete here
    if (payload != nullptr) {
        delete[] payload;
    }
}