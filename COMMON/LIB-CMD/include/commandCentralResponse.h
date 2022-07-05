#include <cstdlib>
#include <iostream>
#include <sstream>
#include <map>

#pragma once
#include "Poco/Logger.h"
#include "commandCentral.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"

class CommandCentralResponse
{

public:

    enum StatusCentralResponse {
        OK,
        KO,
        UNKNOWN
    };

    CommandCentralResponse(std::string uuidFromCommand, CommandCentral::CommandCentralType type);
    ~CommandCentralResponse();

    void setStatus(StatusCentralResponse status) {this->status = status;}
    void setComments(std::string comments) {this->comments = comments;}
    void setDatas(std::string datas) {this->datas = datas;}
    void updateTimestamp() {timestamp = Poco::Timestamp().epochMicroseconds();}

    std::string getUuid() {return uuid;}
    StatusCentralResponse getStatus() {return status;}
    CommandCentral::CommandCentralType getType() {return type;}
    std::string getDatas() {return datas;}
    std::string getComments() {return comments;}
    uint64_t getTimestamp() {return timestamp;}

protected:

    // -- define the status
    StatusCentralResponse status;

    // -- type of response identify with command type
    CommandCentral::CommandCentralType type;

    // -- action of what happened if OK or KO
    std::string comments;

    // -- additional datas as XML 
    std::string datas;

    // -- to delete unused responses
    uint64_t timestamp;

private:

    // -- identify response with the request uuid
    std::string uuid;
};