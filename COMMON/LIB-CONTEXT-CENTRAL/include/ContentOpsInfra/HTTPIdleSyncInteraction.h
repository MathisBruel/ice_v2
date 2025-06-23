#pragma once
#include "HTTPInteraction.h"
#include "ContentOpsApp/IdleSyncInteraction.h"

class HTTPIdleSyncInteraction : public HTTPInteraction, public IdleSyncInteraction
{
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) override {
        return IdleSyncInteraction::pfStateIdleSync(cmdUUID, Params);
    }
    void Run() override { IdleSyncInteraction::pfTransitionToCPL(); }
    void Run(bool) override { IdleSyncInteraction::pfTransitionToSyncLoop(); }
};