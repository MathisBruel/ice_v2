#pragma once
#include "ContentOpsApp/TransitionResponse.h"
#include <functional>
#include <map>
#include <string>

class ReleaseInteraction
{
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateReleaseCreation;
    std::function<void()> pfTransitionToPublishing;
};