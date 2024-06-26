#pragma once
#include "HTTPInteraction.h"
#include "App/Release_Interaction.h"

class HTTPReleaseInteraction : public HTTPInteraction
{
private:
    Release_Interaction* StateInteraction;
public:
    HTTPReleaseInteraction() {this->StateInteraction = nullptr;}
    HTTPReleaseInteraction(Release_Interaction* Interactor) {this->StateInteraction = Interactor;}
    ~HTTPReleaseInteraction() {this->StateInteraction = nullptr;}

    void SetInteractor(Release_Interaction* Interactor) {this->StateInteraction = Interactor;}
    void run() override { 
        (StateInteraction->State->*(StateInteraction->pfTransition))();
    }
};