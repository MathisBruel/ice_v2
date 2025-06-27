#pragma once
#include <iostream>
#include <map>
#include "machine.hpp"
#include "Context.h"
#include "ContentOpsInfra/MySQLContentRepo.h"
#include "ContentOpsInfra/MySQLReleaseRepo.h"
#include "ContentOpsInfra/MySQLCISRepo.h"
#include "ContentOpsInfra/MySQLCPLRepo.h"
#include "ContentOpsInfra/MySQLSyncRepo.h"
#include "ContentOpsInfra/MySQLSyncLoopRepo.h"
#include "ContentOpsApp/Context.h"


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