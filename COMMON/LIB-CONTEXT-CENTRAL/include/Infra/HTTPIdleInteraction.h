#pragma once
#include "HTTPInteraction.h"
#include "App/IdleInteraction.h"

class HTTPIdleInteraction : public HTTPInteraction, public IdleInteraction
{
    void Run() override { 
        IdleInteraction::pfTransition();
    }
};
