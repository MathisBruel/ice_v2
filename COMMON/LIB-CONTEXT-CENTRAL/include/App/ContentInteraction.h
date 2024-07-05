#pragma once
#include "App/State.h"
class ContentInteraction
{
public:
    std::function<TransitionResponse(std::string, std::string)> pfTransitionToPublishing;
};