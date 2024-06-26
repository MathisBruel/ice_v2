#pragma once
#include "App/State.h"
class Release_Interaction
{
public:
    State_ReleaseCreation* State;
    void (State_ReleaseCreation::*pfTransition)();
};