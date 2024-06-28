#pragma once
#include "App/State.h"
class SyncInteraction
{
public:
    std::function<void()> pfTransition;
};