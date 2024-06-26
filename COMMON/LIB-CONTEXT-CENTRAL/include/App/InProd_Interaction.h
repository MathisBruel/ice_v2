#pragma once
#include "App/State.h"
class InProd_Interaction
{
public:
    State_InProd* State;
    void (State_InProd::*pfTransition)();
};