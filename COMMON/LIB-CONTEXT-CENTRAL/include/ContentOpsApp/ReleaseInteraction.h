#pragma once
#include "ContentOpsApp/State.h"
class ReleaseInteraction
{
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateReleaseCreation;
    std::function<void()> pfTransitionToPublishing;
};