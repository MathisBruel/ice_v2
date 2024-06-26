#pragma once
#include "HTTPInteraction.h"
#include "App/IdleSync_Interaction.h"

class HTTPIdleSyncInteraction : public HTTPInteraction
{
private:
    bool SyncLoop;
    IdleSync_Interaction* StateInteraction;
public:
    HTTPIdleSyncInteraction() { StateInteraction = nullptr; SyncLoop = false; }
    HTTPIdleSyncInteraction(IdleSync_Interaction* Interactor) { this->StateInteraction = Interactor; }
    HTTPIdleSyncInteraction(IdleSync_Interaction* Interactor, bool SyncLoop) { this->StateInteraction = Interactor; this->SyncLoop = SyncLoop;}
    ~HTTPIdleSyncInteraction() { StateInteraction = nullptr; }

    void SetInteractor(IdleSync_Interaction* Interactor) { StateInteraction = Interactor; }
    void SetSyncLoop(bool SyncLoop) { this->SyncLoop = SyncLoop; }
    void run() override { 
        if (SyncLoop) { (StateInteraction->State->*(StateInteraction->pfTransitionToSYNCLOOP))(); }
        else { (StateInteraction->State->*(StateInteraction->pfTransitionToCPL))(); }
    }
};