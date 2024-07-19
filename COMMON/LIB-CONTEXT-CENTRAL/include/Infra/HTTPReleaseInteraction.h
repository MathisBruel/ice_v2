#pragma once
#include "HTTPInteraction.h"
#include "App/ReleaseInteraction.h"

class HTTPReleaseInteraction : public HTTPInteraction, public ReleaseInteraction
{
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) override {
        return ReleaseInteraction::pfStateReleaseCreation(cmdUUID, Params);
    }
    void Run() override { ReleaseInteraction::pfTransitionToPublishing(); }
};