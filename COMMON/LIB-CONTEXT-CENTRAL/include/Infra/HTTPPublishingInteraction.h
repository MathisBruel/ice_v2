#pragma once
#include "HTTPInteraction.h"
#include "App/PublishingInteraction.h"

class HTTPPublishingInteraction : public HTTPInteraction, public PublishingInteraction
{
    void Run() override { 
        PublishingInteraction::pfTransition();
    }
};