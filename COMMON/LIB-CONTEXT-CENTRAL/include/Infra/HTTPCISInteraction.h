#pragma once
#include "HTTPInteraction.h"
#include "App/CISInteraction.h"

class HTTPCISInteraction : public HTTPInteraction, public CISInteraction
{
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) override {
        return CISInteraction::pfStateUploadCIS(cmdUUID, Params);
    }
};