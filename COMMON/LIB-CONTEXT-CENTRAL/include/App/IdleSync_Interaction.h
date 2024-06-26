#pragma once
#include "App/State.h"
class IdleSync_Interaction
{
public:
    State_IdleSync* State;
    void (State_IdleSync::*pfTransitionToCPL)();
    void (State_IdleSync::*pfTransitionToSYNCLOOP)();
};