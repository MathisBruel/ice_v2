#pragma once
#include "HTTPInteraction.h"
#include "App/Idle_Interaction.h"

class HTTPIdleInteraction : public HTTPInteraction
{
private:
    Idle_Interaction* StateInteraction;
public:
    HTTPIdleInteraction() : StateInteraction(nullptr) {}
    HTTPIdleInteraction(Idle_Interaction* Interactor) : StateInteraction(Interactor) {}
    ~HTTPIdleInteraction() { StateInteraction = nullptr; }

    void SetInteractor(Idle_Interaction* Interactor) { StateInteraction = Interactor; }
    void run() override { 
        (StateInteraction->State->*(StateInteraction->pfTransition))();
    }
};
