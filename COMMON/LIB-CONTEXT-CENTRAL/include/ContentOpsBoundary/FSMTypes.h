#pragma once
#include <machine.hpp>
#include "ContentOpsBoundary/COB_Context.h"

using Config = hfsm2::Config::ContextT<COB_Context*>;
using Machine = hfsm2::MachineT<Config>;
using BoundaryFSM = Machine; 

// Déclarations forward des états
struct StateCreateRelease;
struct StateArchiveRelease;
struct StateUploadCIS;
struct StateUploadSyncLoop;
struct StateWaitCloseRelease;
struct StateGotNewCPL;

using BoundaryStateMachineFSM = BoundaryFSM::Root<
    StateCreateRelease,
    BoundaryFSM::Orthogonal<
        StateUploadCIS,
        StateUploadSyncLoop,
        StateGotNewCPL
    >,
    StateWaitCloseRelease,
    StateArchiveRelease
>; 