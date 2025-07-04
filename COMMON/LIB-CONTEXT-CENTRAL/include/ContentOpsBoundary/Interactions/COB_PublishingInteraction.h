#pragma once
#include "COB_Interaction.h"
#include "ContentOpsApp/PublishingInteraction.h"

class COB_PublishingInteraction : public COB_Interaction, public PublishingInteraction {
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStatePublishing(cmdUUID, Params);
    }
    void Run() { pfTransitionToReleaseCreation(); }
    void Run(bool) { pfTransitionToCancel(); }
}; 