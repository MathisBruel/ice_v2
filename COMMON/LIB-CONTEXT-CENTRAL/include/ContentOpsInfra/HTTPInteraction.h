#pragma once
#include "ContentOpsApp/TransitionResponse.h"

class HTTPInteraction {
public:
    virtual TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) { return TransitionResponse(); };
    virtual void Run() {};
    virtual void Run(bool) {};
};