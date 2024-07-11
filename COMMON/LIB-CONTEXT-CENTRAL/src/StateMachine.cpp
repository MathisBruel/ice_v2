#define HFSM2_ENABLE_STRUCTURE_REPORT
 
#include "App/StateMachine.h"
 
struct StateTemplate : FSM::State {
    template <typename Event>
    void react(const Event&, EventControl&) {}
    TransitionResponse response;
};
 
struct StateContentInit : StateTemplate {
    using StateTemplate::react;
    void entryGuard(FullControl& control)  {       
        control.context().contentInteraction->pfStateContentInit = [control,this](std::string UUID, std::map<std::string, std::string> Params){
            this->response.cmdUUID = UUID;
            newContent(control, Params["contentTitle"]);
            if (control.context().content) {
                this->response.cmdStatus = "OK";
                this->response.cmdComment = "Content created";
                this->response.cmdDatasXML = control.context().content->toXmlString();
                return this->response;
            }
            this->response.cmdStatus = "KO";
            this->response.cmdComment = "Failed Content create";
            return this->response;
        };
    }
    void newContent(Control control, std::string contentTitle) {
        control.context().content = new Content(contentTitle);
        MySQLContentRepo* contentRepo = new MySQLContentRepo() ;
        ResultQuery* result = control.context().dbConnection->ExecuteQuery(contentRepo->MySQLcreate(control.context().content));
        control.context().content->SetContentId(42);
    }
    void react(const ContentInitEvent&, EventControl& control) {
        control.changeTo<StatePublishing>();
    }

};
struct StatePublishing : StateTemplate {
    using StateTemplate::react;
    FullControl* stateControl;
    void entryGuard(FullControl& control)  {
        control.context().publishingInteraction->pfStatePublishing = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
            this->response.cmdUUID = UUID;
            this->response.cmdStatus = "OK";
            this->response.cmdComment = "Publishing";
            this->response.cmdDatasXML = control.context().content->toXmlString();
            return this->response;
        };
    }
    void update(FullControl& control)  {
        std::cout << "Etats d'avancement: CIS " << control.context().cisFinish << " Sync " << control.context().syncFinish << "\n";
    }
    void react(const CancelEvent&, EventControl& control) {
        control.changeTo<StateCancel>();
    }
    void react(const CreateReleaseEvent&, EventControl& control) {
        control.changeTo<StateReleaseCreation>();
    }
};
 
struct StateReleaseCreation : StateTemplate {
    using StateTemplate::react;
    void entryGuard(FullControl& control)  {
        control.context().releaseInteraction->pfStateReleaseCreation = [control,this](std::string UUID, std::map<std::string, std::string> Params) {
            this->response.cmdUUID = UUID;
            this->response.cmdStatus = "OK";
            this->response.cmdComment = "Release created";
            this->response.cmdDatasXML = control.context().content->toXmlString();
            return this->response;
        };
    }
    void react(const ReleaseCreatedEvent&, EventControl& control) {
        control.resume<StatePublishing>();
    }
}; 

struct StateUploadCIS : StateTemplate {
    using StateTemplate::react;
    void entryGuard(FullControl& control)  {
        control.context().cisInteraction->pfStateUploadCIS = [control, this] (std::string UUID, std::map<std::string, std::string> Params) {
            this->response.cmdUUID = UUID;
            this->response.cmdStatus = "OK";
            this->response.cmdComment = "CIS uploaded";
            this->response.cmdDatasXML = control.context().content->toXmlString();
            return this->response;
        };
    }
    void react(const PushCISEvent&, EventControl& control) {
        control.context().cisFinish = true;
        std::cout << "UploadCIS Finish\n";
        if (control.context().cisFinish && control.context().syncFinish) {
            control.changeTo<StateInProd>();
        }
    }
}; 

struct StateSyncCreate : StateTemplate {
    void enter(Control&)  { std::cout << "SyncCreate\n"; }
};
 
struct StateIdleSync : StateTemplate {
    using StateTemplate::react;
    void entryGuard(FullControl& control)  {
        control.context().idleSyncInteraction->pfStateIdleSync = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
            this->response.cmdUUID = UUID;
            this->response.cmdStatus = "OK";
            this->response.cmdComment = "IdleSync";
            this->response.cmdDatasXML = control.context().content->toXmlString();
            return this->response;
        };
    }
    void react(const CreateSyncEvent&, EventControl& control) {
        control.changeTo<StateSyncLoop>();    
    }
    void react(const CreateCPLEvent&, EventControl& control) {
        control.changeTo<StateCPL>();    
    }
};
 
struct StateCPL : StateTemplate {
    using StateTemplate::react;
    void entryGuard(FullControl& control)  {
        control.context().cplInteraction->pfStateCPL = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
            this->response.cmdUUID = UUID;
            this->response.cmdStatus = "OK";
            this->response.cmdComment = "CPL";
            this->response.cmdDatasXML = control.context().content->toXmlString();
            return this->response;
        };
    }
    void react (const CreateSyncEvent&, EventControl& control) {
        control.changeTo<StateSync>();
    }
};
 
struct StateSync : StateTemplate {
    using StateTemplate::react;
    void entryGuard(FullControl& control)  {
        control.context().syncInteraction->pfStateSync = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
            this->response.cmdUUID = UUID;
            this->response.cmdStatus = "OK";
            this->response.cmdComment = "Sync";
            this->response.cmdDatasXML = control.context().content->toXmlString();
            return this->response;
        };
    }
    void react (const SyncCreatedEvent&, EventControl& control) {
        control.context().syncFinish = true;
        std::cout << "SyncCreated Finish\n";
        if (control.context().cisFinish && control.context().syncFinish) {
            control.changeTo<StateInProd>();
        }
    }
};
 
struct StateSyncLoop : StateTemplate {
    using StateTemplate::react;
    void entryGuard(FullControl& control)  {
        control.context().syncLoopInteraction->pfStateSyncLoop = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
            this->response.cmdUUID = UUID;
            this->response.cmdStatus = "OK";
            this->response.cmdComment = "SyncLoop";
            this->response.cmdDatasXML = control.context().content->toXmlString();
            return this->response;
        };
    }
    void react (const SyncCreatedEvent&, EventControl& control) {
        control.context().syncFinish = true;
        std::cout << "SyncCreate Finish\n";
        if (control.context().cisFinish && control.context().syncFinish) {
            control.changeTo<StateInProd>();
        }  
    }
};
 
struct StateCancel : StateTemplate {
    void enter(Control&)  { std::cout << "Cancel\n"; }
};
 
struct StateInProd : StateTemplate {
    using StateTemplate::react;
    void entryGuard(FullControl& control)  {
        control.context().inProdInteraction->pfStateInProd = [control, this] (std::string UUID, std::map<std::string, std::string> Params) {
            this->response.cmdUUID = UUID;
            this->response.cmdStatus = "OK";
            this->response.cmdComment = "InProd";
            this->response.cmdDatasXML = control.context().content->toXmlString();
            return this->response;
        };
    }
};

StateMachine::StateMachine(Context* context) {
    this->_fsmInstance = new FSM::Instance (*context);
}
 
FSM::Instance * StateMachine::GetFSM() {
    return this->_fsmInstance;
}
 
void StateMachine::Transition(StateEvent eventTrigger) {
    switch (eventTrigger)
    {
    case StateEvent::ContentInit:
        this->_fsmInstance->react(ContentInitEvent{});
        break;
    case StateEvent::CreateCPL:
        this->_fsmInstance->react(CreateCPLEvent{});
        break;
    case StateEvent::CreateSync:
        this->_fsmInstance->react(CreateSyncEvent{});
        break;
    case StateEvent::SyncCreated:
        this->_fsmInstance->react(SyncCreatedEvent{});
        break;
    case StateEvent::PushCIS:
        this->_fsmInstance->react(PushCISEvent{});
        break;
    case StateEvent::Publish:
        this->_fsmInstance->react(PublishEvent{});
        break;
    case StateEvent::CreateRelease:
        this->_fsmInstance->react(CreateReleaseEvent{});
        break;
    case StateEvent::ReleaseCreated:
        this->_fsmInstance->react(ReleaseCreatedEvent{});
        break;
    case StateEvent::Cancel:
        this->_fsmInstance->react(CancelEvent{});
        break; 
    }
}