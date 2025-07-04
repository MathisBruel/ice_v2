#pragma once
#include "ContentOpsApp/SyncInteraction.h"

class COB_SyncInteraction : public SyncInteraction {
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStateSync(cmdUUID, Params);
    }
    void Run() { pfTransitionToInProduction(); }
}; 