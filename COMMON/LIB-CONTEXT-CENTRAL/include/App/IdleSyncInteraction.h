#pragma once
#include "App/State.h"
class IdleSyncInteraction
{
public:
    std::function<TransitionResponse(std::string, std::map<std::string,std::string>)> pfStateIdleSync;
    std::function<void()> pfTransitionToCPL;
    std::function<void()> pfTransitionToSyncLoop;
};