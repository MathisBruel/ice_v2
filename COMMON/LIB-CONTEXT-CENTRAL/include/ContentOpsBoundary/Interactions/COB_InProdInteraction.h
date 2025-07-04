#pragma once
#include "COB_Interaction.h"
#include "ContentOpsApp/InProdInteraction.h"

class COB_InProdInteraction : public COB_Interaction, public InProdInteraction {
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStateInProd(cmdUUID, Params);
    }
}; 