#pragma once
#include "HTTPInteraction.h"
#include "ContentOpsApp/PublishingInteraction.h"

class HTTPPublishingInteraction : public HTTPInteraction, public PublishingInteraction
{
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) override { 
        return PublishingInteraction::pfStatePublishing(cmdUUID, Params);
    }
    void Run() override { PublishingInteraction::pfTransitionToReleaseCreation(); };
    void Run(bool) override { PublishingInteraction::pfTransitionToCancel(); };
};