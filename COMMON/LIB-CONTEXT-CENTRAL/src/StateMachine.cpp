#define HFSM2_ENABLE_LOG_INTERFACE
#define HFSM2_ENABLE_STRUCTURE_REPORT
#define HFSM2_ENABLE_TRANSITION_HISTORY

#include <iostream>
#include "machine.hpp" 
#include "App/StateMachine.h"

struct Context
{
    bool CISFinish = false;
    bool SyncFinish = false;
};

using M = hfsm2::MachineT<hfsm2::Config::ContextT<Context>>;

using FSM = M::PeerRoot<
    State_Idle,
    State_ContentInit,
    M::Orthogonal<State_Publishing,
    State_UploadCIS,
    M::Composite<State_SyncCreate,
    State_IdleSync,
    State_CPL,
    State_Sync,
    State_SyncLoop
    >
    >,
    State_ReleaseCreation,
    State_Cancel,
    State_InProd
>;

static_assert(FSM::regionId<State_Publishing>() == 1, "");
static_assert(FSM::regionId<State_SyncCreate>() == 2, "");

static_assert(FSM::stateId<State_Idle>() == 1, "");
static_assert(FSM::stateId<State_ContentInit>() == 2, "");
static_assert(FSM::stateId<State_Publishing>() == 3, "");
static_assert(FSM::stateId<State_UploadCIS>() == 4, "");
static_assert(FSM::stateId<State_SyncCreate>() == 5, "");
static_assert(FSM::stateId<State_IdleSync>() == 6, "");
static_assert(FSM::stateId<State_CPL>() == 7, "");
static_assert(FSM::stateId<State_Sync>() == 8, "");
static_assert(FSM::stateId<State_SyncLoop>() == 9, "");
static_assert(FSM::stateId<State_ReleaseCreation>() == 10, "");
static_assert(FSM::stateId<State_Cancel>() == 11, "");
static_assert(FSM::stateId<State_InProd>() == 12, "");

struct Logger : M::LoggerInterface
{
    static const char* stateName(const StateID stateId) {
        switch (stateId) {
        case 1:
            return "Idle";
        case 2:
            return "ContentInit";
        case 3:
            return "Publishing";
        case 4:
            return "UploadCIS";
        case 5:
            return "SyncCreate";
        case 6:
            return "IdleSync";
        case 7:
            return "CPL";
        case 8:
            return "Sync";
        case 9:
            return "SyncLoop";
        case 10:
            return "ReleaseCreation";
        case 11:
            return "Cancel";
        case 12:
            return "InProd";
        default:
            return "";
        }
    }

    void recordTransition(const Context& /*context*/,
        const StateID origin,
        const TransitionType /*transition*/,
        const StateID target) override {
        std::cout << "T: " << stateName(origin) << " -> " << stateName(target) << "\n";
    }
};

struct Base : FSM::State {
    template <typename Event>
    void react(const Event&, EventControl&) {}
};

struct State_Idle : Base {
    static FSM::Instance* stateMachine;
    using Base::react;
    void enter(Control&)  { std::cout << "Idle\n"; }
    void react(const InitContent&, EventControl& control)  {
        control.changeTo<State_ContentInit>();
    }
};

struct State_ContentInit : Base {
    using Base::react;
    void enter(Control& control)  {
        control.context().CISFinish = false;
        control.context().SyncFinish = false;
        std::cout << "ContentInit\n";
    }
    void react(const Publish&, EventControl& control)  {
        control.changeTo<State_Publishing>();
    }
};

struct State_ReleaseCreation : Base {
    using Base::react;
    void enter(Control&)  { std::cout << "ReleaseCreated\n"; }
    void react(const ReleaseCreated&, EventControl& control)  {
        control.resume<State_Publishing>();
    }
};

struct State_Publishing : Base {
    using Base::react;
    void enter(Control&)  { std::cout << "Publishing\n"; }
    void update(FullControl& control)  {
        std::cout << "Etats d'avancement: CIS " << control.context().CISFinish << " Sync " << control.context().SyncFinish << "\n";
    }
    void react(const CreateRelease&, EventControl& control)  {
        control.changeTo<State_ReleaseCreation>();
    }
};

struct State_UploadCIS : Base {
    using Base::react;
    void enter(Control&)  { std::cout << "UploadCIS\n"; }
    void react(const Upload&, EventControl& control)  {
        control.context().CISFinish = true;
        std::cout << "UploadCIS Finish\n";
        if (control.context().CISFinish && control.context().SyncFinish) {
            control.changeTo<State_InProd>();
        }
    }
};

struct State_SyncCreate : Base {
    using Base::react;
    void enter(Control&)  { std::cout << "SyncCreate\n"; }
};

struct State_IdleSync : Base {
    using Base::react;
    void enter(Control&)  { std::cout << "IdleSync\n"; }
    void react(const CreateCPL&, EventControl& control)  {
        control.changeTo<State_CPL>();
    }
    void react(const CreateSyncLoop&, EventControl& control)  {
        control.changeTo<State_SyncLoop>();
    }
};

struct State_CPL : Base {
    using Base::react;
    void enter(Control&)  { std::cout << "CPL\n"; }
    void react(const CreateSync&, EventControl& control)  {
        control.changeTo<State_Sync>();
    }
};

struct State_Sync : Base {
    using Base::react;
    void enter(Control&)  { std::cout << "Sync\n"; }
    void react(const SyncCreated&, EventControl& control)  {
        control.context().SyncFinish = true;
        std::cout << "SyncCreated Finish\n";
        if (control.context().CISFinish && control.context().SyncFinish) {
            control.changeTo<State_InProd>();
        }
    }
};

struct State_SyncLoop : Base {
    using Base::react;
    void enter(Control& control)  { 
        std::cout << "SyncLoop\n";
    }
    void react(const SyncCreated&, EventControl& control)  {
        control.context().SyncFinish = true;
        std::cout << "SyncCreate Finish\n";
        if (control.context().CISFinish && control.context().SyncFinish) {
            control.changeTo<State_InProd>();
        }
    }
};

struct State_Cancel : Base {
    using Base::react;
    void enter(Control&)  { std::cout << "Cancel\n"; }
};

struct State_InProd : Base {
    using Base::react;
    void enter(Control&)  { std::cout << "InProd\n"; }
    void react(const Stop&, EventControl& control)  {
        control.changeTo<State_Idle>();
    }
};

// Definition of static member
FSM::Instance* State_Idle::stateMachine = nullptr;

int main() {
    Context context;
    Logger logger;
    FSM::Instance machine(context, &logger);
    State_Idle::stateMachine = &machine;

    std::cout << "Start n°1\n";
    machine.reset();
    machine.react(InitContent{});
    machine.react(Publish{});
    machine.react(CreateCPL{});
    machine.react(Upload{});
    machine.react(CreateSync{});
    machine.update();
    machine.react(CreateRelease{});
    machine.react(ReleaseCreated{});
    machine.update();
    machine.react(SyncCreated{});
    machine.react(Stop{});

    std::cout << "\nStart n°2\n";
    machine.reset();
    machine.react(InitContent{});
    machine.react(Publish{});
    machine.react(CreateSyncLoop{});
    machine.react(SyncCreated{});
    machine.react(Upload{});
    machine.react(Stop{});

    return 0;
}
