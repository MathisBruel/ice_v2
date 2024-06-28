#pragma once
#include "HTTPInteraction.h"
#include "App/IdleSyncInteraction.h"

class HTTPIdleSyncInteraction : public HTTPInteraction, public IdleSyncInteraction
{
private:
    bool _syncLoop;
public:
    void SetSyncLoop(bool syncLoop) { this->_syncLoop = syncLoop; }
    void Run() override { 
        if (_syncLoop) { IdleSyncInteraction::pfTransitionToSYNCLOOP(); }
        else { IdleSyncInteraction::pfTransitionToCPL(); }
    }
};