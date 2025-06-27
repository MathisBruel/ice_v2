#pragma once
#include "ContentOpsApp/State.h"
class SyncLoopInteraction
{
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateSyncLoop;
    std::function<void()> pfTransitionToInProduction;
};