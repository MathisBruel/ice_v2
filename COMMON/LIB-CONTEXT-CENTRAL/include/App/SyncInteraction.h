#pragma once
#include "App/State.h"
class SyncInteraction
{
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateSync;
};