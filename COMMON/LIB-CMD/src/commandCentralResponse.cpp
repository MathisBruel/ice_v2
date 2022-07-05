#include "commandCentralResponse.h"

CommandCentralResponse::CommandCentralResponse(std::string uuidFromCommand, CommandCentral::CommandCentralType type)
{
    this->uuid = uuidFromCommand;
    this->type = type;
    this->datas = "";
    this->status = UNKNOWN;
    timestamp = Poco::Timestamp().epochMicroseconds();
}
CommandCentralResponse::~CommandCentralResponse()
{

}