#include <cstdlib>
#include <iostream>
#include <sstream>
#include <map>

#pragma once
#include "Poco/Logger.h"
#include "command.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"

class CommandResponse
{

public:

    enum StatusResponse {
        OK,
        KO,
        UNKNOWN
    };

    CommandResponse(std::string uuidFromCommand, Command::CommandType type);
    ~CommandResponse();

    void setStatus(StatusResponse status) {this->status = status;}
    void setComments(std::string comments) {this->comments = comments;}
    void setDatas(std::string datas) {this->datas = datas;}
    void updateTimestamp() {timestamp = Poco::Timestamp().epochMicroseconds();}

    std::string getUuid() {return uuid;}
    StatusResponse getStatus() {return status;}
    Command::CommandType getType() {return type;}
    std::string getDatas() {return datas;}
    std::string getComments() {return comments;}
    uint64_t getTimestamp() {return timestamp;}

    virtual void fillDatas() {datas = "";}

protected:

    // -- define the status
    StatusResponse status;

    // -- type of response identify with command type
    Command::CommandType type;

    // -- action of what happened if OK or KO
    std::string comments;

    // -- additional datas as XML 
    std::string datas;

    // -- to delete unused responses
    uint64_t timestamp;

    bool isFile;

private:

    // -- identify response with the request uuid
    std::string uuid;
};