#pragma once
#include "App/State.h"
class PublishingInteraction
{
public:
    std::function<void()> pfTransition;
};