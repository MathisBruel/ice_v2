#pragma once
#include "ContentOpsApp/State.h"
class ContentInteraction
{
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateContentInit;
    std::function<void()> pfTransitionToPublishing;
};