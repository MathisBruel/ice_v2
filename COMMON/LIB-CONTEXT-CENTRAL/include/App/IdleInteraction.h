#pragma once
#include "App/State.h"
class IdleInteraction
{
public:
    std::function<void()> pfTransition;
};