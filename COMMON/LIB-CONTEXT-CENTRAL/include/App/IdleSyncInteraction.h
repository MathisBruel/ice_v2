#pragma once
#include "App/State.h"
class IdleSyncInteraction
{
public:
    std::function<void()> pfTransitionToCPL;
    std::function<void()> pfTransitionToSYNCLOOP;
};