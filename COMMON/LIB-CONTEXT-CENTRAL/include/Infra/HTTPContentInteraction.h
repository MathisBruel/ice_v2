#pragma once
#include "HTTPInteraction.h"
#include "App/ContentInteraction.h"

class HTTPContentInteraction : public HTTPInteraction , public ContentInteraction
{
public:
    void Run() override { 
        ContentInteraction::pfTransition();
    }
};