#pragma once
#include "HTTPInteraction.h"
#include "App/Publishing_Interaction.h"

class HTTPPublishingInteraction
: public HTTPInteraction, public PublishingInteraction
{
public:
    void run() { (this->state->*pfTransition)();}
}