#include <cstdlib>
#include <iostream>

#pragma once
#include "Poco/Timestamp.h"
#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"

class Session
{

public:

    Session();
    ~Session();

    void reinitTimeout();
    void handleTime();
    bool isTimeOut();

    std::string getId() {return id;}

private:

    std::string id;
    long timeoutMicro;
    long timestamp;
};