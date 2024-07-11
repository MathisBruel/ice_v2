#pragma once
#include "App/State.h"
class SyncLoopInteraction
{
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateSyncLoop;
};