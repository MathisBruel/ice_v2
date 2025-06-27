#pragma once
#include "HTTPInteraction.h"
#include "ContentOpsApp/CISInteraction.h"

class HTTPCISInteraction : public HTTPInteraction, public CISInteraction
{
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) override {
        return CISInteraction::pfStateUploadCIS(cmdUUID, Params);
    }
    void Run() override { CISInteraction::pfTransitionToInProduction(); }
};