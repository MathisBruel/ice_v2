#pragma once
#include "HTTPInteraction.h"
#include "App/Publishing_Interaction.h"

class HTTPPublishingInteraction : public HTTPInteraction
{
private:
    Publishing_Interaction* StateInteraction;
public:
    HTTPPublishingInteraction() {this->StateInteraction = nullptr;}
    HTTPPublishingInteraction(Publishing_Interaction* Interactor) {this->StateInteraction = Interactor;}
    ~HTTPPublishingInteraction() {this->StateInteraction = nullptr;}

    void SetInteractor(Publishing_Interaction* Interactor) {this->StateInteraction = Interactor;}
    void run() override { 
        (StateInteraction->State->*(StateInteraction->pfTransition))();
    }
};