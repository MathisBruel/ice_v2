#include "App/StateMachine.h"
class SyncLoop_Interaction
{
private:
    void (State_SyncLoop::*pfTransition)(Const SyncCreated&, EventControl&) = &State_SyncLoop::react;
public:
    SyncLoop_Interaction();
    ~SyncLoop_Interaction();
};