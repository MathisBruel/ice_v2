#pragma once
#include "ContentOpsApp/SyncLoopInteraction.h"

class COB_SyncLoopInteraction : public SyncLoopInteraction {
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStateSyncLoop(cmdUUID, Params);
    }
    void Run() { pfTransitionToInProduction(); }
}; 