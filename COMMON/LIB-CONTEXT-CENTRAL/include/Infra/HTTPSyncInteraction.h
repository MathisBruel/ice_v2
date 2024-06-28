#pragma once
#include "HTTPInteraction.h"
#include "App/SyncInteraction.h"

class HTTPSyncInteraction : public HTTPInteraction, public SyncInteraction
{
    void Run() override { 
        SyncInteraction::pfTransition();
    }
};