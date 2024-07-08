#pragma once
#include <iostream>
#include <map>
#include "machine.hpp"
#include "Context.h"
 
using M = hfsm2::MachineT<hfsm2::Config::ContextT<Context>>;
 
using FSM = M::PeerRoot<
    StateIdle,
    StateContentInit,
    M::Orthogonal<StatePublishing,
        StateUploadCIS,
        M::Composite<StateSyncCreate,
        StateIdleSync,
        StateCPL,
        StateSync,
        StateSyncLoop
        >
    >,
    StateReleaseCreation,
    StateCancel,
    StateInProd
>;
class StateMachine {
    private:
        FSM::Instance * _fsmInstance;
    public:
        StateMachine(Context* context);
        FSM::Instance * GetFSM();
        void Transition(std::string eventTrigger);
};