#pragma once
#include "App/State.h"
class SyncLoopInteraction
{
public:
    std::function<void()> pfTransition;
};