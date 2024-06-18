#pragma once
#include "App/State.h"
class CIS_Interaction
{
// protected:
//     State_UploadCIS* state;
//     CIS_Interaction(State_UploadCIS& state) { this->state = &state; }
public:
    void (State_UploadCIS::*pfTransition)();
};