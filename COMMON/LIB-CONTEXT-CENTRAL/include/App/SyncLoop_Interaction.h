#pragma once
#include "App/State.h"
class SyncLoop_Interaction
{
// protected:
//     State_SyncLoop* state;
//     SyncLoop_Interaction(State_SyncLoop& state) { this->state = &state; }
public:
    void (State_SyncLoop::*pfTransition)();
};