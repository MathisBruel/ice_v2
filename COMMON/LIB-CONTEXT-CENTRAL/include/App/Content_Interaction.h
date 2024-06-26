#pragma once
#include "App/State.h"
class Content_Interaction
{
public:
    State_ContentInit* State;
    void (State_ContentInit::*pfTransition)();
};