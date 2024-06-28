#pragma once
#include "App/State.h"
class CISInteraction
{
public:
    std::function<void()> pfTransition;
};