#pragma once
#include "HTTPInteraction.h"
#include "App/ReleaseInteraction.h"

class HTTPReleaseInteraction : public HTTPInteraction, public ReleaseInteraction
{
    void Run() override { 
        ReleaseInteraction::pfTransition();
    }
};