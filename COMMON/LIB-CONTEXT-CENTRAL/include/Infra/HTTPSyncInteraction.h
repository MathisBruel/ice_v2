#pragma once
#include "HTTPInteraction.h"
#include "App/Sync_Interaction.h"

class HTTPSyncInteraction : public HTTPInteraction
{
private:
    Sync_Interaction* StateInteraction;
public:
    HTTPSyncInteraction() {this->StateInteraction = nullptr;}
    HTTPSyncInteraction(Sync_Interaction* Interactor) {this->StateInteraction = Interactor;}
    ~HTTPSyncInteraction() {this->StateInteraction = nullptr;}

    void SetInteractor(Sync_Interaction* Interactor) {this->StateInteraction = Interactor;}
    void run() override { 
        (StateInteraction->State->*(StateInteraction->pfTransition))();
    }
};