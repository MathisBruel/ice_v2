#define HFSM2_ENABLE_LOG_INTERFACE
#define HFSM2_ENABLE_STRUCTURE_REPORT
#define HFSM2_ENABLE_TRANSITION_HISTORY

#include "App/StateMachine.h"

using M = hfsm2::MachineT<hfsm2::Config::ContextT<Context>>;

// Event 
struct InitContent {};
struct Publish {};
struct CreateCPL {};
struct CreateSync {};
struct CreateSyncLoop {};
struct SyncCreated {};
struct Upload {};
struct CreateRelease {};
struct ReleaseCreated {};
struct Stop {};

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

    void recordTransition(const Context&,
        const StateID origin,
        const TransitionType,
        const StateID target) {
        std::cout << "T: " << stateName(origin) << " -> " << stateName(target) << "\n";
    }
};

struct StateTemplate : FSM::State {
    FullControl* StateControl;
};

struct State_Idle : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::StateControl = &control;
        StateTemplate::StateControl->context().idleInteraction->State = this;
        StateTemplate::StateControl->context().idleInteraction->pfTransition = &State_Idle::Transition;
    }
    void enter(Control&)  { std::cout << "Idle\n"; }
    void Transition()  {
        StateTemplate::StateControl->context().idleInteraction->pfTransition = nullptr;
        StateTemplate::StateControl->changeTo<State_Publishing>();
    }
};

struct State_ContentInit : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::StateControl = &control;
        StateTemplate::StateControl->context().contentInteraction->State = this;
        StateTemplate::StateControl->context().contentInteraction->pfTransition = &State_ContentInit::Transition;
        StateTemplate::StateControl->context().CISFinish = false;
        StateTemplate::StateControl->context().SyncFinish = false;
    }
    void enter(Control&)  { std::cout << "ContentInit\n"; }
    void Transition() {
        StateTemplate::StateControl->context().contentInteraction->pfTransition = nullptr;
        StateTemplate::StateControl->changeTo<State_Publishing>();    
    }
};

struct State_ReleaseCreation : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::StateControl = &control;
        StateTemplate::StateControl->context().releaseInteraction->State = this;
        StateTemplate::StateControl->context().releaseInteraction->pfTransition = &State_ReleaseCreation::Transition;
    }
    void enter(Control&)  { std::cout << "ReleaseCreated\n"; }   
    void Transition() {
        StateTemplate::StateControl->context().releaseInteraction->pfTransition = nullptr;
        StateTemplate::StateControl->resume<State_Publishing>();    
    }
};

struct State_Publishing : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::StateControl = &control;
        StateTemplate::StateControl->context().publishingInteraction->State = this;
        StateTemplate::StateControl->context().publishingInteraction->pfTransition = &State_Publishing::Transition;
    }
    void enter(Control&)  { std::cout << "Publishing\n"; }
    void update(FullControl& control)  {
        std::cout << "Etats d'avancement: CIS " << control.context().CISFinish << " Sync " << control.context().SyncFinish << "\n";
    }
    void Transition() {
        StateTemplate::StateControl->context().publishingInteraction->pfTransition = nullptr;
        StateTemplate::StateControl->changeTo<State_ReleaseCreation>();    
    }
};

struct State_UploadCIS : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::StateControl = &control;
        StateTemplate::StateControl->context().cisInteraction->State = this;
        StateTemplate::StateControl->context().cisInteraction->pfTransition = &State_UploadCIS::Transition;
    }
    void enter(Control&)  { std::cout << "UploadCIS\n"; }
    void Transition() {
        StateTemplate::StateControl->context().cisInteraction->pfTransition = nullptr;
        StateTemplate::StateControl->context().CISFinish = true;
        std::cout << "UploadCIS Finish\n";
        if (StateTemplate::StateControl->context().CISFinish && StateTemplate::StateControl->context().SyncFinish) {
            StateTemplate::StateControl->changeTo<State_InProd>();
        }
    }
};

struct State_SyncCreate : StateTemplate {
    void enter(Control&)  { std::cout << "SyncCreate\n"; }
};

struct State_IdleSync : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::StateControl = &control;
        StateTemplate::StateControl->context().idleSyncInteraction->State = this;
        StateTemplate::StateControl->context().idleSyncInteraction->pfTransitionToCPL = &State_IdleSync::TransitionToCPL;
        StateTemplate::StateControl->context().idleSyncInteraction->pfTransitionToSYNCLOOP = &State_IdleSync::TransitionToSYNCLOOP;
    }
    void enter(Control&)  { std::cout << "IdleSync\n"; }
    void TransitionToSYNCLOOP() {
        StateTemplate::StateControl->context().idleSyncInteraction->pfTransitionToCPL = nullptr;
        StateTemplate::StateControl->context().idleSyncInteraction->pfTransitionToSYNCLOOP = nullptr;
        StateTemplate::StateControl->changeTo<State_SyncLoop>();    
    }
    void TransitionToCPL() {
        StateTemplate::StateControl->context().idleSyncInteraction->pfTransitionToCPL = nullptr;
        StateTemplate::StateControl->context().idleSyncInteraction->pfTransitionToSYNCLOOP = nullptr;
        StateTemplate::StateControl->changeTo<State_CPL>();    
    }
};

struct State_CPL : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::StateControl = &control;
        StateTemplate::StateControl->context().cplInteraction->State = this;
        StateTemplate::StateControl->context().cplInteraction->pfTransition = &State_CPL::Transition;
    }
    void enter(Control&)  { std::cout << "CPL\n"; }
    void Transition() {
        StateTemplate::StateControl->context().cplInteraction->pfTransition = nullptr;
        StateTemplate::StateControl->changeTo<State_Sync>();    
    }
};

struct State_Sync : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::StateControl = &control;
        StateTemplate::StateControl->context().syncInteraction->State = this;
        StateTemplate::StateControl->context().syncInteraction->pfTransition = &State_Sync::Transition;
    }
    void enter(Control&)  { std::cout << "Sync\n"; }
    void Transition() {
        StateTemplate::StateControl->context().syncInteraction->pfTransition = nullptr;
        StateTemplate::StateControl->context().SyncFinish = true;
        std::cout << "SyncCreated Finish\n";
        if (StateTemplate::StateControl->context().CISFinish && StateTemplate::StateControl->context().SyncFinish) {
            StateTemplate::StateControl->changeTo<State_InProd>();
        }
    }
};

struct State_SyncLoop : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::StateControl = &control;
        StateTemplate::StateControl->context().syncloopInteraction->State = this;
        StateTemplate::StateControl->context().syncloopInteraction->pfTransition = &State_SyncLoop::Transition;
    }
    void enter(Control&)  { std::cout << "SyncLoop\n"; }
    void Transition() {
        StateTemplate::StateControl->context().syncloopInteraction->pfTransition = nullptr;
        StateTemplate::StateControl->context().SyncFinish = true;
        std::cout << "SyncCreate Finish\n";
        if (StateTemplate::StateControl->context().CISFinish && StateTemplate::StateControl->context().SyncFinish) {
            StateTemplate::StateControl->changeTo<State_InProd>();
        }   
    }
};

struct State_Cancel : StateTemplate {
    void enter(Control&)  { std::cout << "Cancel\n"; }
};

struct State_InProd : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::StateControl = &control;
        StateTemplate::StateControl->context().inProdInteraction->State = this;
        StateTemplate::StateControl->context().inProdInteraction->pfTransition = &State_InProd::Transition;
    }
    void enter(Control&)  { std::cout << "InProd\n"; }
    void Transition() {
        StateTemplate::StateControl->context().inProdInteraction->pfTransition = nullptr;
        StateTemplate::StateControl->changeTo<State_Idle>();    
    }
};

StateMachine::StateMachine(Context context) {
    Logger logger;
    FSM::Instance machine(context, &logger);

    // std::cout << "Start n°1\n";
    // machine.reset();
    // machine.react(InitContent{});
    // machine.react(Publish{});
    // machine.react(CreateCPL{});
    // machine.react(Upload{});
    // machine.react(CreateSync{});
    // machine.update();
    // machine.react(CreateRelease{});
    // machine.react(ReleaseCreated{});
    // machine.update();
    // machine.react(SyncCreated{});
    // machine.react(Stop{});

    // std::cout << "\nStart n°2\n";
    // machine.reset();
    // machine.react(InitContent{});
    // machine.react(Publish{});
    // machine.react(CreateSyncLoop{});
    // machine.react(SyncCreated{});
    // machine.react(Upload{});
    // machine.react(Stop{});
}