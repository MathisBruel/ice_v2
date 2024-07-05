#pragma once
#include "HTTPInteraction.h"
#include "App/IdleInteraction.h"

class HTTPIdleInteraction : public HTTPInteraction, public IdleInteraction
{
public:
    void Run() override { 
        IdleInteraction::pfTransitionToContentInit();
    }
};
