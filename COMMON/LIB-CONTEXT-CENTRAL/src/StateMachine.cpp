#define HFSM2_ENABLE_STRUCTURE_REPORT
#define HFSM2_ENABLE_TRANSITION_HISTORY
 
#include "App/StateMachine.h"
 
struct StateTemplate : FSM::State {
    FullControl* stateControl;
};
 
struct StateIdle : StateTemplate {
    using FSM::State::react;
    void entryGuard(FullControl& control)  {
        /*control.context().idleInteraction->pfTransitionToContentInit = [control](){
            FullControl changeControl = control; // cast FullControlT to FullControl for using the methode changeTo
            changeControl.changeTo<StateContentInit>();
        };*/
    }
    void react(ContentCreatedEvent& event, EventControl& control) {
        std::cout << "ContentCreatedEvent\n";
        control.changeTo<StateContentInit>();
    }
};
 
struct StateContentInit : StateTemplate {
    void entryGuard(FullControl& control)  {
        /*
        control.context().syncFinish = false;
       
        control.context().contentInteraction->pfTransitionToPublishing = [this,control](std::string UUID, std::string contentTitle){
            FullControl changeControl = control; // cast FullControlT to FullControl for using the methode changeTo
            changeControl.changeTo<StatePublishing>();
 
            NewContent(changeControl, contentTitle);
 
            TransitionResponse response;
            response.cmdUUID = UUID;
            response.cmdStatus = "OK";
            response.cmdComment = "Content Created";
            response.cmdDatasXML = changeControl.context().content->toXmlString();
            return response;
        };
            */
        std::cout << "ContentInit\n";
    }
    void NewContent(FullControl changeControl, std::string contentTitle) {
        changeControl.context().content = new Content(contentTitle);
    }
};
 
struct StateReleaseCreation : StateTemplate {
    void entryGuard(FullControl& control)  {
        StateTemplate::stateControl = &control;
        StateTemplate::stateControl->context().releaseInteraction->pfTransition = std::bind(&StateReleaseCreation::Transition, this);
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
        StateTemplate::stateControl->context().publishingInteraction->pfTransition = std::bind(&StatePublishing::Transition, this);
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
        StateTemplate::stateControl->context().cisInteraction->pfTransition = std::bind(&StateUploadCIS::Transition, this);
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
        StateTemplate::stateControl->context().cplInteraction->pfTransition = std::bind(&StateCPL::Transition, this);
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
        StateTemplate::stateControl->context().syncInteraction->pfTransition = std::bind(&StateSync::Transition, this);
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
        StateTemplate::stateControl->context().syncloopInteraction->pfTransition = std::bind(&StateSyncLoop::Transition, this);
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
 
StateMachine::StateMachine(Context* context) {
    FSM::Instance machine(*context);
    this->_fsmInstance = &machine;
}
 
FSM::Instance * StateMachine::GetFSM() {
    return this->_fsmInstance;
}
 
void StateMachine::Transition(std::string eventTrigger) {
    if (eventTrigger == "ContentCreatedEvent") {
        this->_fsmInstance->react(ContentCreatedEvent{});
    } 
}