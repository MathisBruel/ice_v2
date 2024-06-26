#pragma once
#include "App/State.h"
class SyncLoop_Interaction
{
public:
    State_SyncLoop* State;
    void (State_SyncLoop::*pfTransition)();
};