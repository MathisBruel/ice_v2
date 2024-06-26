#pragma once
#include "App/State.h"
class Publishing_Interaction
{
public:
    State_Publishing* State;
    void (State_Publishing::*pfTransition)();
};