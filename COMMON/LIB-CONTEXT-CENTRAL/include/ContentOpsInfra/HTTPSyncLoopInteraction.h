#pragma once
#include "HTTPInteraction.h"
#include "ContentOpsApp/SyncLoopInteraction.h"

class HTTPSyncLoopInteraction : public HTTPInteraction, public SyncLoopInteraction
{
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) override {
        return SyncLoopInteraction::pfStateSyncLoop(cmdUUID, Params);
    }
    void Run() override { SyncLoopInteraction::pfTransitionToInProduction(); }
};