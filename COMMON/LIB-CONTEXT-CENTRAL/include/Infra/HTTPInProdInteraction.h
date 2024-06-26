#pragma once
#include "HTTPInteraction.h"
#include "App/InProd_Interaction.h"

class HTTPInProdInteraction : public HTTPInteraction
{
private:
    InProd_Interaction* StateInteraction;
public:
    HTTPInProdInteraction() {this->StateInteraction = nullptr;}
    HTTPInProdInteraction(InProd_Interaction* Interactor) {this->StateInteraction = Interactor;}
    ~HTTPInProdInteraction() {this->StateInteraction = nullptr;}

    void SetInteractor(InProd_Interaction* Interactor) {this->StateInteraction = Interactor;}
    void run() override { 
        (StateInteraction->State->*(StateInteraction->pfTransition))();
    }
};