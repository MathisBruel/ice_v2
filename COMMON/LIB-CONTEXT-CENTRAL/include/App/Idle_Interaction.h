#pragma once
#include "App/State.h"
class Idle_Interaction
{
public:
    State_Idle* State;
    void (State_Idle::*pfTransition)();
};