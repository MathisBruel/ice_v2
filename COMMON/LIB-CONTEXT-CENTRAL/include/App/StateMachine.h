#pragma once
#include <iostream>
#include <map>
#include "machine.hpp"
#include "Context.h"
#include "Infra/MySQLContentRepo.h"
#include "Infra/MySQLReleaseRepo.h"
#include "Infra/MySQLCISRepo.h"
#include "Infra/MySQLCPLRepo.h"
#include "Infra/MySQLSyncRepo.h"
#include "Infra/MySQLSyncLoopRepo.h"
#include "App/Context.h"


using M = hfsm2::MachineT<hfsm2::Config::ContextT<Context>>;

using FSM = M::PeerRoot<
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
        void Transition(StateEvent eventTrigger);
};