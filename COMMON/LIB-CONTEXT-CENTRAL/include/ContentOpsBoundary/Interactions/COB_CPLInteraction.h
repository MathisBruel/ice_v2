#pragma once
#include "COB_Interaction.h"
#include "ContentOpsApp/CPLInteraction.h"

class COB_CPLInteraction : public COB_Interaction, public CPLInteraction {
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStateCPL(cmdUUID, Params);
    }
    void Run() { pfTransitionToSync(); }
}; 