#pragma once
#include "HTTPInteraction.h"
#include "App/Sync_Interaction.h"

class HTTPSyncInteraction
: public HTTPInteraction, public Sync_Interaction
{
public:
    void run() { (this->state->*pfTransition)();}
}