#pragma once
#include "HTTPInteraction.h"
#include "App/ContentInteraction.h"

class HTTPContentInteraction : public HTTPInteraction, public ContentInteraction
{
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) override { 
        return ContentInteraction::pfStateContentInit(cmdUUID, Params);
    }
};