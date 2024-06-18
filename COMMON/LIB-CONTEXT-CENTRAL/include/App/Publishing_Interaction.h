#pragma once
#include "App/State.h"
class Publishing_Interaction
{
// protected:
//     State_Publishing* state;
//     Publishing_Interaction(State_Publishing& state) { this->state = &state; }
public:
    void (State_Publishing::*pfTransition)();
};