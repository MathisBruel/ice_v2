#include "App/StateMachine.h"
class Release_Interaction
{
private:
    void (State_ReleaseCreation::*pfTransition)(Const ReleaseCreated&, EventControl&) = &State_ReleaseCreation::react;
public:
    Release_Interaction();
    ~Release_Interaction();
};