#pragma once
#include "HTTPInteraction.h"
#include "App/Release_Interaction.h"

class HTTPReleaseInteraction : 
public HTTPInteraction, public ReleaseInteraction
{
public:
    void run() { (this->state->*pfTransition)();}
}