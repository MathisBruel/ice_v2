#pragma once
#include "App/State.h"
class Release_Interaction
{
// protected:
//     State_ReleaseCreation* state;
//     Release_Interaction(State_ReleaseCreation& state) { this->state = &state; }
public:
    void (State_ReleaseCreation::*pfTransition)();
};