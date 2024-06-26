#pragma once
#include "HTTPInteraction.h"
#include "App/CPL_Interaction.h"

class HTTPCPLInteraction : public HTTPInteraction
{
private:
    CPL_Interaction* StateInteraction;
public:
    HTTPCPLInteraction() {this->StateInteraction = nullptr;}
    HTTPCPLInteraction(CPL_Interaction* Interactor) {this->StateInteraction = Interactor;}
    ~HTTPCPLInteraction() {this->StateInteraction = nullptr;}

    void SetInteractor(CPL_Interaction* Interactor) {this->StateInteraction = Interactor;}
    void run() override { 
        (StateInteraction->State->*(StateInteraction->pfTransition))();
    }
};