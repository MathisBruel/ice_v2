#pragma once
#include "HTTPInteraction.h"
#include "App/CISInteraction.h"

class HTTPCISInteraction : public HTTPInteraction, public CISInteraction
{
    void Run() override { 
        CISInteraction::pfTransition();
    }
};