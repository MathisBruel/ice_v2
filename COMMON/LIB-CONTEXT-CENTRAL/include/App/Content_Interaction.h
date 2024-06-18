#pragma once
#include "App/State.h"
class Content_Interaction
{
// protected:
//     State_ContentInit* state;
//     Content_Interaction(State_ContentInit& state) { this->state = &state; }
public:
    void (State_ContentInit::*pfTransition)();
};