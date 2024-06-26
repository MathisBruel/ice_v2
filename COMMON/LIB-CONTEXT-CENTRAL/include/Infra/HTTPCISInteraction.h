#pragma once
#include "HTTPInteraction.h"
#include "App/CIS_Interaction.h"

class HTTPCISInteraction : public HTTPInteraction
{
private:
    CIS_Interaction* StateInteraction;
public:
    HTTPCISInteraction() {this->StateInteraction = nullptr;}
    HTTPCISInteraction(CIS_Interaction* Interactor) {this->StateInteraction = Interactor;}
    ~HTTPCISInteraction() {this->StateInteraction = nullptr;}

    void SetInteractor(CIS_Interaction* Interactor) {this->StateInteraction = Interactor;}
    void run() override { 
        (StateInteraction->State->*(StateInteraction->pfTransition))();
    }
};