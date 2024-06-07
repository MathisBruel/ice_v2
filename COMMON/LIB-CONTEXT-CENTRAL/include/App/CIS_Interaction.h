#include "App/StateMachine.h"
class CIS_Interaction
{
private:
    void (State_UploadCIS::*pfTransition)(const Upload&, EventControl&) = &State_UploadCIS::react;
public:
    CIS_Interaction();
    ~CIS_Interaction();
};