#pragma once
#include "App/State.h"
class CIS_Interaction
{
public:
    State_UploadCIS* State;
    void (State_UploadCIS::*pfTransition)();
};