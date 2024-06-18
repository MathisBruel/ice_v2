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

    void recordTransition(const Context& /*context*/,
        const StateID origin,
        const TransitionType /*transition*/,
        const StateID target) {
        std::cout << "T: " << stateName(origin) << " -> " << stateName(target) << "\n";
    }
};

struct Base : FSM::State {
    template <typename Event>
    // Control* StateControl;
    void react(const Event&, EventControl&) {}
};

struct State_Idle : Base {
    using Base::react;

    void enter(Control&)  { std::cout << "Idle\n"; }
    void react(const InitContent&, EventControl& control)  {
        control.changeTo<State_ContentInit>();
    }
};

struct State_ContentInit : Base {
    using Base::react;
    void enter(Control& control)  {
        // this->StateControl = &control;
        // control.context().contentInteraction.pfTransition = &State_ContentInit::Transition;
        control.context().CISFinish = false;
        control.context().SyncFinish = false;
        std::cout << "ContentInit\n";
    }
    void react(const Publish&, EventControl& control)  {
        control.changeTo<State_Publishing>();
    }
    // void Transition() {
    //     this->StateControl->changeTo<State_Publishing>();    
    // }
};

struct State_ReleaseCreation : Base {
    using Base::react;
    void enter(Control& control)  { 
        std::cout << "ReleaseCreated\n"; 
        // this->StateControl = &control; 
        // control.context().releaseInteraction.pfTransition = &State_ReleaseCreation::Transition;
    }
    void react(const ReleaseCreated&, EventControl& control)  {
        control.resume<State_Publishing>();
    }    
    // void Transition() {
    //     this->StateControl->resume<State_Publishing>();    
    // }
};

struct State_Publishing : Base {
    using Base::react;
    void enter(Control& control)  { 
        std::cout << "Publishing\n"; 
        // this->StateControl = &control;
        // control.context().publishingInteraction.pfTransition = &State_Publishing::Transition;

    }
    void update(FullControl& control)  {
        std::cout << "Etats d'avancement: CIS " << control.context().CISFinish << " Sync " << control.context().SyncFinish << "\n";
    }
    void react(const CreateRelease&, EventControl& control)  {
        control.changeTo<State_ReleaseCreation>();
    }
    // void Transition() {
    //     this->StateControl->changeTo<State_ReleaseCreation>();    
    // }
};

struct State_UploadCIS : Base {
    using Base::react;
    void enter(Control& control)  { 
        std::cout << "UploadCIS\n"; 
        // this->StateControl = &control;
        // control.context().cisInteraction.pfTransition = &State_UploadCIS::Transition;
    }

    void react(const Upload&, EventControl& control)  {
        control.context().CISFinish = true;
        std::cout << "UploadCIS Finish\n";
        if (control.context().CISFinish && control.context().SyncFinish) {
            control.changeTo<State_InProd>();
        }
    }
    // void Transition() {
    //     this->StateControl->context().cisInteraction.pfTransition = nullptr;
    //     this->StateControl->context().CISFinish = true;
    //     std::cout << "UploadCIS Finish\n";
    //     if (this->StateControl->context().CISFinish && this->StateControl->context().SyncFinish) {
    //         this->StateControl->changeTo<State_InProd>();
    //     }
    // }
};

struct State_SyncCreate : Base {
    using Base::react;
    // void enter(Control& control)  { std::cout << "SyncCreate\n"; this->StateControl = &control;}
};

struct State_IdleSync : Base {
    using Base::react;
    void enter(Control& control)  { 
        std::cout << "IdleSync\n"; 
        // this->StateControl = &control;
        // control.context().idleSyncInteraction.pfTransitionToCPL = &State_IdleSync::TransitionToCPL;
        // control.context().idleSyncInteraction.pfTransitionToSYNCLOOP = &State_IdleSync::TransitionToSYNCLOOP;
    }
    void react(const CreateCPL&, EventControl& control)  {
        control.changeTo<State_CPL>();
    }
    void react(const CreateSyncLoop&, EventControl& control)  {
        control.changeTo<State_SyncLoop>();
    }
    // void TransitionToSYNCLOOP() {
    //     this->StateControl->changeTo<State_SyncLoop>();    
    // }
    // void TransitionToCPL() {
    //     this->StateControl->changeTo<State_CPL>();    
    // }
};

struct State_CPL : Base {
    using Base::react;
    void enter(Control& control)  { 
        std::cout << "CPL\n"; 
        // this->StateControl = &control;
        // control.context().cplInteraction.pfTransition = &State_CPL::Transition;

    }
    void react(const CreateSync&, EventControl& control)  {
        control.changeTo<State_Sync>();
    }
    // void Transition() {
    //     this->StateControl->changeTo<State_Sync>();    
    // }
};

struct State_Sync : Base {
    using Base::react;
    void enter(Control& control)  { 
        std::cout << "Sync\n"; 
        // this->StateControl = &control;
        // control.context().syncInteraction.pfTransition = &State_Sync::Transition;
    }
    void react(const SyncCreated&, EventControl& control)  {
        control.context().SyncFinish = true;
        std::cout << "SyncCreated Finish\n";
        if (control.context().CISFinish && control.context().SyncFinish) {
            control.changeTo<State_InProd>();
        }
    }
    // void Transition() {
    //     this->StateControl->context().SyncFinish = true;
    //     std::cout << "SyncCreated Finish\n";
    //     if (this->StateControl->context().CISFinish && this->StateControl->context().SyncFinish) {
    //         this->StateControl->changeTo<State_InProd>();
    //     }
    // }
};

struct State_SyncLoop : Base {
    using Base::react;
    void enter(Control& control)  { 
        std::cout << "SyncLoop\n";
        // this->StateControl = &control;
        // control.context().syncloopInteraction.pfTransition = &State_SyncLoop::Transition;
    }
    void react(const SyncCreated&, EventControl& control)  {
        control.context().SyncFinish = true;
        std::cout << "SyncCreate Finish\n";
        if (control.context().CISFinish && control.context().SyncFinish) {
            control.changeTo<State_InProd>();
        }
    }
    // void Transition() {
    //     this->StateControl->context().SyncFinish = true;
    //     std::cout << "SyncCreate Finish\n";
    //     if (this->StateControl->context().CISFinish && this->StateControl->context().SyncFinish) {
    //         this->StateControl->changeTo<State_InProd>();
    //     }   
    // }
};

struct State_Cancel : Base {
    using Base::react;
    void enter(Control&)  { std::cout << "Cancel\n"; }
};

struct State_InProd : Base {
    using Base::react;
    void enter(Control& control)  { 
        std::cout << "InProd\n"; 
        // this->StateControl = &control;
        // control.context().contentInteraction.pfTransition = &State_InProd::Transition;
    }
    void react(const Stop&, EventControl& control)  {
        control.changeTo<State_Idle>();
    }
    // void Transition() {
    //     this->StateControl->changeTo<State_Idle>();    
    // }
};

int main() {
    Context context;
    Logger logger;
    FSM::Instance machine(context, &logger);

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