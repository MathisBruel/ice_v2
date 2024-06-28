#pragma once
#include "HTTPInteraction.h"
#include "App/SyncLoopInteraction.h"

class HTTPSyncLoopInteraction : public HTTPInteraction, public SyncLoopInteraction
{
public:
    void Run() override { 
        SyncLoopInteraction::pfTransition();
    }
};