#pragma once
#include "HTTPInteraction.h"
#include "App/SyncInteraction.h"

class HTTPSyncInteraction : public HTTPInteraction, public SyncInteraction
{
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) override {
        return SyncInteraction::pfStateSync(cmdUUID, Params);
    }
    void Run() override { SyncInteraction::pfTransitionToInProduction(); }
};