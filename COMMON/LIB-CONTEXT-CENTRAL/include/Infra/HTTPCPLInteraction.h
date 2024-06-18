#pragma once
#include "HTTPInteraction.h"
#include "App/CPL_Interaction.h"

class HTTPCPLInteraction :
public HTTPInteraction, public CPL_Interaction
{
public:
    void run() { (this->state->*pfTransition)();}
}