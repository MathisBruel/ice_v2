#pragma once
#include "COB_Interaction.h"
#include "ContentOpsApp/ContentInteraction.h"

class COB_ContentInteraction : public COB_Interaction, public ContentInteraction {
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStateContentInit(cmdUUID, Params);
    }
    void Run() { pfTransitionToPublishing(); }
}; 