#pragma once
#include "ContentOpsApp/InProdInteraction.h"

class COB_InProdInteraction : public InProdInteraction {
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStateInProd(cmdUUID, Params);
    }
}; 