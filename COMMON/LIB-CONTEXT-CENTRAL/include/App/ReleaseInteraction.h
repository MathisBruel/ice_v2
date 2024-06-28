#pragma once
#include "App/State.h"
class ReleaseInteraction
{
public:
    std::function<void()> pfTransition;
};