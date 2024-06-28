#pragma once
#include "HTTPInteraction.h"
#include "App/InProdInteraction.h"

class HTTPInProdInteraction : public HTTPInteraction, public InProdInteraction
{
    void Run() override { 
        InProdInteraction::pfTransition();
    }
};