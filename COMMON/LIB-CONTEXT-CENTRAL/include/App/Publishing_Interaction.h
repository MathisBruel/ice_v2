#include "App/StateMachine.h"
class Publishing_Interaction
{
private:
    void (State_Publishing::*pfTransition)(Const CreateRelease&, EventControl&) = &State_Publishing::react;
public:
    Publishing_Interaction();
    ~Publishing_Interaction();
};