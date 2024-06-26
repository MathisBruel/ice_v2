#pragma once
#include "App/State.h"
class Sync_Interaction
{
public:
    State_Sync* State;
    void (State_Sync::*pfTransition)();
};