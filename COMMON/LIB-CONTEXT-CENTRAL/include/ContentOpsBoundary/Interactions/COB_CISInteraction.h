#pragma once
#include "COB_Interaction.h"
#include "ContentOpsApp/CISInteraction.h"
class COB_CISInteraction : public COB_Interaction, public CISInteraction {
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStateUploadCIS(cmdUUID, Params);
    }
    void Run() { pfTransitionToInProduction(); }
}; 