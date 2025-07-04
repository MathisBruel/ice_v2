#pragma once
#include "ContentOpsApp/PublishingInteraction.h"

class COB_PublishingInteraction : public PublishingInteraction {
public:
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStatePublishing(cmdUUID, Params);
    }
    void Run() { pfTransitionToReleaseCreation(); }
    void Run(bool) { pfTransitionToCancel(); }
}; 