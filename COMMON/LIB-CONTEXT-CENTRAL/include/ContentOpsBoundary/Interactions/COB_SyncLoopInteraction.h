#pragma once
#include "COB_Interaction.h"
#include "ContentOpsApp/SyncLoopInteraction.h"

class COB_SyncLoopInteraction : public COB_Interaction, public SyncLoopInteraction {
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStateSyncLoop(cmdUUID, Params);
    }
    void Run() { pfTransitionToInProduction(); }
}; 