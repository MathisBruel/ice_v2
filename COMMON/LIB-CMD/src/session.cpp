#include "session.h"

Session::Session()
{
    timestamp = Poco::Timestamp().epochMicroseconds();
    timeoutMicro = 60000000;
    Poco::UUIDGenerator& generator = Poco::UUIDGenerator::defaultGenerator();
    id = generator.create().toString();
}
Session::~Session()
{

}

void Session::reinitTimeout()
{
    timestamp = Poco::Timestamp().epochMicroseconds();
}

bool Session::isTimeOut()
{
    long currentTimestamp = Poco::Timestamp().epochMicroseconds();
    return currentTimestamp > (timestamp + timeoutMicro);
}