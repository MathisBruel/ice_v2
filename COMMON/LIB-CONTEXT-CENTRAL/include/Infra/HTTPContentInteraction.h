#pragma once
#include "HTTPInteraction.h"
#include "App/Content_Interaction.h"

class HTTPContentInteraction : public HTTPInteraction
{
private:
    Content_Interaction* StateInteraction;
public:
    HTTPContentInteraction() {this->StateInteraction = nullptr;}
    HTTPContentInteraction(Content_Interaction* Interactor) {this->StateInteraction = Interactor;}
    ~HTTPContentInteraction() {this->StateInteraction = nullptr;}

    void SetInteractor(Content_Interaction* Interactor) {this->StateInteraction = Interactor;}
    void run() override { 
        (StateInteraction->State->*(StateInteraction->pfTransition))();
    }
};