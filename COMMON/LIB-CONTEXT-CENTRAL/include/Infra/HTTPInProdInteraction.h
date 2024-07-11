#pragma once
#include "HTTPInteraction.h"
#include "App/InProdInteraction.h"

class HTTPInProdInteraction : public HTTPInteraction, public InProdInteraction
{
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) override {
        return InProdInteraction::pfStateInProd(cmdUUID, Params);
    }
};