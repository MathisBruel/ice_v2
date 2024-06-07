#include "App/StateMachine.h"
class Content_Interaction
{
public:
    void (State_ContentInit::*pfTransition)(const Publish&, EventControl&) = State_ContentInit::react;
    Content_Interaction();
    ~Content_Interaction();
};