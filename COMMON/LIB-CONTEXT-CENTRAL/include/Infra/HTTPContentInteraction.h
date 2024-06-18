#pragma once
#include "HTTPInteraction.h"
#include "App/Content_Interaction.h"

class HTTPContentInteraction : 
    public HTTPInteraction, public Content_Interaction
{
public:
    void run() { (this->state->*pfTransition)();}
}