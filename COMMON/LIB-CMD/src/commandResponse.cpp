#include "commandResponse.h"

CommandResponse::CommandResponse(std::string uuidFromCommand, Command::CommandType type)
{
    this->uuid = uuidFromCommand;
    this->type = type;
    this->datas = "";
    this->status = UNKNOWN;
    timestamp = Poco::Timestamp().epochMicroseconds();
}
CommandResponse::~CommandResponse()
{

}