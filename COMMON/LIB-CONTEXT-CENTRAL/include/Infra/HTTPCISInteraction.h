#pragma once
#include "HTTPInteraction.h"
#include "App/CIS_Interaction.h"

class HTTPCISInteraction : 
public HTTPInteraction, public CIS_Interaction
{
public:
    void run() { (this->state->*pfTransition)();}
};