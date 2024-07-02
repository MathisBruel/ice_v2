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
    FullControl* stateControl;
    std::map<std::string, std::string> params;
};

struct StateIdle : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::stateControl = &control;
        StateTemplate::stateControl->context().idleInteraction->pfTransition = std::bind(&StateIdle::Transition, this);
    }
    void enter(Control&)  { std::cout << "Idle\n"; }
    void Transition()  {
        StateTemplate::stateControl->context().idleInteraction->pfTransition = nullptr;
        StateTemplate::stateControl->changeTo<StateContentInit>();
    }
};

struct StateContentInit : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::stateControl = &control;
        StateTemplate::stateControl->context().contentInteraction->pfTransition = std::bind(&StateContentInit::Transition,this);
        StateTemplate::stateControl->context().cisFinish = false;
        StateTemplate::stateControl->context().syncFinish = false;
    }
    void enter(Control&)  { std::cout << "ContentInit\n"; }
    void Transition() {
        StateTemplate::stateControl->context().contentInteraction->pfTransition = nullptr;
        StateTemplate::stateControl->changeTo<StatePublishing>();    
    }
};

struct StateReleaseCreation : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::stateControl = &control;
        StateTemplate::stateControl->context().releaseInteraction->pfTransition = std::bind(&StateReleaseCreation::Transition,this);
    }
    void enter(Control&)  { std::cout << "ReleaseCreated\n"; }   
    void Transition() {
        StateTemplate::stateControl->context().releaseInteraction->pfTransition = nullptr;
        StateTemplate::stateControl->resume<StatePublishing>();    
    }
};

struct StatePublishing : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::stateControl = &control;
        StateTemplate::stateControl->context().publishingInteraction->pfTransition = std::bind(&StatePublishing::Transition,this);
    }
    void enter(Control&)  { std::cout << "Publishing\n"; }
    void update(FullControl& control)  {
        std::cout << "Etats d'avancement: CIS " << control.context().cisFinish << " Sync " << control.context().syncFinish << "\n";
    }
    void Transition() {
        StateTemplate::stateControl->context().publishingInteraction->pfTransition = nullptr;
        StateTemplate::stateControl->changeTo<StateReleaseCreation>();    
    }
};

struct StateUploadCIS : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::stateControl = &control;
        StateTemplate::stateControl->context().cisInteraction->pfTransition = std::bind(&StateUploadCIS::Transition,this);
    }
    void enter(Control&)  { std::cout << "UploadCIS\n"; }
    void Transition() {
        StateTemplate::stateControl->context().cisInteraction->pfTransition = nullptr;
        StateTemplate::stateControl->context().cisFinish = true;
        std::cout << "UploadCIS Finish\n";
        if (StateTemplate::stateControl->context().cisFinish && StateTemplate::stateControl->context().syncFinish) {
            StateTemplate::stateControl->changeTo<StateInProd>();
        }
    }
};

struct StateSyncCreate : StateTemplate {
    void enter(Control&)  { std::cout << "SyncCreate\n"; }
};

struct StateIdleSync : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::stateControl = &control;
        StateTemplate::stateControl->context().idleSyncInteraction->pfTransitionToCPL = std::bind(&StateIdleSync::TransitionToCPL,this);
        StateTemplate::stateControl->context().idleSyncInteraction->pfTransitionToSYNCLOOP = std::bind(&StateIdleSync::TransitionToSYNCLOOP,this);
    }
    void enter(Control&)  { std::cout << "IdleSync\n"; }
    void TransitionToSYNCLOOP() {
        StateTemplate::stateControl->context().idleSyncInteraction->pfTransitionToCPL = nullptr;
        StateTemplate::stateControl->context().idleSyncInteraction->pfTransitionToSYNCLOOP = nullptr;
        StateTemplate::stateControl->changeTo<StateSyncLoop>();    
    }
    void TransitionToCPL() {
        StateTemplate::stateControl->context().idleSyncInteraction->pfTransitionToCPL = nullptr;
        StateTemplate::stateControl->context().idleSyncInteraction->pfTransitionToSYNCLOOP = nullptr;
        StateTemplate::stateControl->changeTo<StateCPL>();    
    }
};

struct StateCPL : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::stateControl = &control;
        StateTemplate::stateControl->context().cplInteraction->pfTransition = std::bind(&StateCPL::Transition,this);
    }
    void enter(Control&)  { std::cout << "CPL\n"; }
    void Transition() {
        StateTemplate::stateControl->context().cplInteraction->pfTransition = nullptr;
        StateTemplate::stateControl->changeTo<StateSync>();    
    }
};

struct StateSync : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::stateControl = &control;
        StateTemplate::stateControl->context().syncInteraction->pfTransition = std::bind(&StateSync::Transition,this);
    }
    void enter(Control&)  { std::cout << "Sync\n"; }
    void Transition() {
        StateTemplate::stateControl->context().syncInteraction->pfTransition = nullptr;
        StateTemplate::stateControl->context().syncFinish = true;
        std::cout << "SyncCreated Finish\n";
        if (StateTemplate::stateControl->context().cisFinish && StateTemplate::stateControl->context().syncFinish) {
            StateTemplate::stateControl->changeTo<StateInProd>();
        }
    }
};

struct StateSyncLoop : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::stateControl = &control;
        StateTemplate::stateControl->context().syncloopInteraction->pfTransition = std::bind(&StateSyncLoop::Transition,this);
    }
    void enter(Control&)  { std::cout << "SyncLoop\n"; }
    void Transition() {
        StateTemplate::stateControl->context().syncloopInteraction->pfTransition = nullptr;
        StateTemplate::stateControl->context().syncFinish = true;
        std::cout << "SyncCreate Finish\n";
        if (StateTemplate::stateControl->context().cisFinish && StateTemplate::stateControl->context().syncFinish) {
            StateTemplate::stateControl->changeTo<StateInProd>();
        }   
    }
};

struct StateCancel : StateTemplate {
    void enter(Control&)  { std::cout << "Cancel\n"; }
};

struct StateInProd : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::stateControl = &control;
        StateTemplate::stateControl->context().inProdInteraction->pfTransition = std::bind(&StateInProd::Transition,this);
    }
    void enter(Control&)  { std::cout << "InProd\n"; }
    void Transition() {
        StateTemplate::stateControl->context().inProdInteraction->pfTransition = nullptr;
        StateTemplate::stateControl->changeTo<StateIdle>();    
    }
};

StateMachine::StateMachine(Context context) {
    Logger logger;
    FSM::Instance machine(context, &logger);
}