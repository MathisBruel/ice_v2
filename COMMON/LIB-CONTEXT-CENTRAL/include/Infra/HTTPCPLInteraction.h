#pragma once
#include "HTTPInteraction.h"
#include "App/CPLInteraction.h"

class HTTPCPLInteraction : public HTTPInteraction, public CPLInteraction
{
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) override {
        return CPLInteraction::pfStateCPL(cmdUUID, Params);
    }
};