#include "App/StateMachine.h"
class IdleSync_Interaction
{
private:
    void (State_IdleSync::*pfTransition)(Const CreateCPL&, EventControl&) = &State_IdleSync::react;
    void (State_IdleSync::*pfTransition)(Const CreateSync&, EventControl&) = &State_IdleSync::react;
public:
    IdleSync_Interaction();
    ~IdleSync_Interaction();
};