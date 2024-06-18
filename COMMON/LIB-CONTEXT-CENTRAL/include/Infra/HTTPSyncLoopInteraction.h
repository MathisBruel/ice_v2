#pragma once
#include "HTTPInteraction.h"
#include "App/SyncLoop_Interaction.h"

class HTTPSyncLoopInteraction
: public HTTPInteraction, public SyncLoop_Interaction
{
public:
    void run() { (this->state->*pfTransition)();}
}