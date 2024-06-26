#pragma once
#include "App/State.h"
class CPL_Interaction
{
public:
    State_CPL* State;
    void (State_CPL::*pfTransition)();
};