#pragma once
#include "App/State.h"
class IdleSync_Interaction
{
// protected:
//     State_IdleSync* state;
//     IdleSync_Interaction(State_IdleSync& state) { this->state = &state; }
public:
    void (State_IdleSync::*pfTransitionToCPL)();
    void (State_IdleSync::*pfTransitionToSYNCLOOP)();
};