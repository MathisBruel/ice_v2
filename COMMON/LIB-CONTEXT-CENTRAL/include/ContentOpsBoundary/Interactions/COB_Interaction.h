#pragma once
#include <map>
#include <string>
#include "ContentOpsApp/TransitionResponse.h"

class COB_Interaction {
public:
    virtual TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) { return TransitionResponse(); };
    virtual void Run() {};
    virtual void Run(bool) {};
}; 