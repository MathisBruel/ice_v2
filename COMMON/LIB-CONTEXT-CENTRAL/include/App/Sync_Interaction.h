#pragma once
#include "App/State.h"
class Sync_Interaction
{
// protected:
//     State_Sync* state;
//     Sync_Interaction(State_Sync& state) { this->state = &state; }
public:
    void (State_Sync::*pfTransition)();
};