#pragma once
#include "App/State.h"
class CPLInteraction
{
public:
    std::function<void()> pfTransition;
};