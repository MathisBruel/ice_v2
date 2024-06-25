#include "App/StateMachine.h"
class Sync_Interaction
{
private:
    void (State_Sync::*pfTransition)(Const SyncCreated&, EventControl&) = &State_Sync::react;
public:
    Sync_Interaction();
    ~Sync_Interaction();
};