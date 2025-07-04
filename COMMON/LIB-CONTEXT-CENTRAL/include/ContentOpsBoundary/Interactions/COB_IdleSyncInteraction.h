#pragma once
#include "COB_Interaction.h"
#include "ContentOpsApp/IdleSyncInteraction.h"

class COB_IdleSyncInteraction : public COB_Interaction, public IdleSyncInteraction {
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStateIdleSync(cmdUUID, Params);
    }
    void Run() { pfTransitionToCPL(); }
    void Run(bool) { pfTransitionToSyncLoop(); }
}; 