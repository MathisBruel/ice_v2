#pragma once
#include "COB_Interaction.h"
#include "ContentOpsApp/ReleaseInteraction.h"

class COB_ReleaseInteraction : public COB_Interaction, public ReleaseInteraction {
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStateReleaseCreation(cmdUUID, Params);
    }
    void Run() { pfTransitionToPublishing(); }
}; 