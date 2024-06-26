#pragma once
#include "HTTPInteraction.h"
#include "App/SyncLoop_Interaction.h"

class HTTPSyncLoopInteraction : public HTTPInteraction
{
private:
    SyncLoop_Interaction* StateInteraction;
public:
    HTTPSyncLoopInteraction() {this->StateInteraction = nullptr;}
    HTTPSyncLoopInteraction(SyncLoop_Interaction* Interactor) {this->StateInteraction = Interactor;}
    ~HTTPSyncLoopInteraction() {this->StateInteraction = nullptr;}

    void SetInteractor(SyncLoop_Interaction* Interactor) {this->StateInteraction = Interactor;}
    void run() override { 
        (StateInteraction->State->*(StateInteraction->pfTransition))();
    }
};