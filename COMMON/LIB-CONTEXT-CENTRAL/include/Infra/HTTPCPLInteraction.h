#pragma once
#include "HTTPInteraction.h"
#include "App/CPLInteraction.h"

class HTTPCPLInteraction : public HTTPInteraction, public CPLInteraction
{
    void Run() override { 
        CPLInteraction::pfTransition();
    }
};