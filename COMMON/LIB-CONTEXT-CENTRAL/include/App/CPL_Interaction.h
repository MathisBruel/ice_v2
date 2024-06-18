#pragma once
#include "App/State.h"
class CPL_Interaction
{
// protected:
//     State_CPL* state;
//     CPL_Interaction(State_CPL& state) { this->state = &state; }
public:
    void (State_CPL::*pfTransition)();
};