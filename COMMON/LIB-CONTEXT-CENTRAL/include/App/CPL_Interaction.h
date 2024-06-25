#include "App/StateMachine.h"
class CPL_Interaction
{
private:
    void (State_CPL::*pfTransition)(Const CreateSync&, EventControl&) = &State_CPL::react;
public:
    CPL_Interaction();
    ~CPL_Interaction();
};