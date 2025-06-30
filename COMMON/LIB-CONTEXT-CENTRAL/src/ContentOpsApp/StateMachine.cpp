#define HFSM2_ENABLE_STRUCTURE_REPORT
 
#include "ContentOpsApp/StateMachine.h"
#include "ContentOpsApp/StateMachineManager.h"
#include "ContentOpsDomain/COD_Content.h"

struct StateTemplate : FSM::State {
    template <typename Event>
    void react(const Event&, EventControl&) {}
    TransitionResponse response;
    void changeRelease(Control control, std::string compositeId) {
        control.context().release = control.context().content->GetRelease(compositeId); 
    }
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
                //TODO: this->response.cmdDatasXML = control.context().content->toXmlString(false);
                return this->response;
            }
            this->response.cmdStatus = "KO";
            this->response.cmdComment = "Failed Content create";
            return this->response;
        };
        control.context().contentInteraction->pfTransitionToPublishing = [control](){
            StateMachineManager::GetInstance()->GetStateMachine(*control.context().content->GetContentId())->Transition(StateEvent::CONTENT_INIT);  
        };
    }
    void newContent(Control control, std::string contentTitle) {
        control.context().content = new COD_Content(contentTitle);
        MySQLContentRepo* contentRepo = new MySQLContentRepo();
        contentRepo->Create(control.context().content);
        ResultQuery* result = control.context().dbConnection->ExecuteQuery(contentRepo->GetQuery());
        control.context().content->SetContentId(result->getLastInsertedId());
        delete result;
        delete contentRepo;
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
            changeRelease(control, Params["id_content"] + "_" + Params["id_type"] + "_" + Params["id_localisation"]);
            if (Params["typeOfElement"] == "RELEASE") {           
                deleteRelease(control, Params["id_content"], Params["id_type"], Params["id_localisation"]);
                this->response.cmdComment = "Release deleted";
            }
            else if (Params["typeOfElement"] == "CPL") {
                this->response.cmdComment = "Cpl deleted";
                deleteSync(control, Params["id_serv_pair_config"], Params["id_content"], Params["id_type"], Params["id_localisation"]);
                deleteCPL(control, Params["id_serv_pair_config"], Params["id_content"], Params["id_type"], Params["id_localisation"]);
            } 
            else if (Params["typeOfElement"] == "SYNC") {
                this->response.cmdComment = "Sync deleted";
                deleteSync(control, Params["id_serv_pair_config"], Params["id_content"], Params["id_type"], Params["id_localisation"]);
            } 
            else if (Params["typeOfElement"] == "SYNCLOOP") {
                this->response.cmdComment = "SyncLoop deleted";
                deleteSyncLoop(control, Params["id_serv_pair_config"], Params["id_content"], Params["id_type"], Params["id_localisation"]);
            }
            this->response.cmdStatus = "OK";
            return this->response;
        };
        control.context().publishingInteraction->pfTransitionToReleaseCreation = [control](){
            StateMachineManager::GetInstance()->GetStateMachine(*control.context().content->GetContentId())->Transition(StateEvent::CREATE_RELEASE);
        };
        control.context().publishingInteraction->pfTransitionToCancel = [control](){
            StateMachineManager::GetInstance()->GetStateMachine(*control.context().content->GetContentId())->Transition(StateEvent::CANCEL);
        };
    }
    void deleteRelease(Control control, std::string id_content, std::string id_type, std::string id_localisation) {
        MySQLReleaseRepo* releaseRepo = new MySQLReleaseRepo();
        releaseRepo->Remove(control.context().release);
        control.context().dbConnection->ExecuteQuery(releaseRepo->GetQuery());
        control.context().content->DeleteRelease(id_content + "_" + id_type + "_" + id_localisation);
        control.context().release = nullptr;
        delete releaseRepo;
    }
    void deleteCPL(Control control, std::string id_serv_pair_config, std::string id_content, std::string id_type, std::string id_localisation) {
        std::string compositeId = id_serv_pair_config + "_" + id_content + "_" + id_type + "_" + id_localisation;
        MySQLCPLRepo* cplRepo = new MySQLCPLRepo();
        cplRepo->Remove(control.context().release->GetCPL(compositeId));
        control.context().dbConnection->ExecuteQuery(cplRepo->GetQuery());
        control.context().release->DeleteCPL(compositeId);
        delete cplRepo;
    }
    void deleteSync(Control control, std::string id_serv_pair_config, std::string id_content, std::string id_type, std::string id_localisation) {
        std::string compositeId = id_serv_pair_config + "_" + id_content + "_" + id_type + "_" + id_localisation;
        MySQLSyncRepo* syncRepo = new MySQLSyncRepo();
        syncRepo->Remove(control.context().release->GetCPL(compositeId)->GetSync());
        control.context().dbConnection->ExecuteQuery(syncRepo->GetQuery());
        control.context().release->GetCPL(compositeId)->DeleteSync();
        delete syncRepo;
    }
    void deleteSyncLoop(Control control, std::string id_serv_pair_config, std::string id_content, std::string id_type, std::string id_localisation) {
        std::string compositeId = id_serv_pair_config + "_" + id_content + "_" + id_type + "_" + id_localisation;
        MySQLSyncLoopRepo* syncLoopRepo = new MySQLSyncLoopRepo();
        syncLoopRepo->Remove(control.context().release->GetSyncLoop(compositeId));
        control.context().dbConnection->ExecuteQuery(syncLoopRepo->GetQuery());
        control.context().release->DeleteSyncLoop(compositeId);
        delete syncLoopRepo;
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
            newRelease(control, std::stoi(Params["id_content"]), std::stoi(Params["id_type"]), std::stoi(Params["id_localisation"]), Params["cplRefPath"]);
            this->response.cmdStatus = "OK";
            this->response.cmdComment = "Release created";
            return this->response;
        };
        control.context().releaseInteraction->pfTransitionToPublishing = [control](){
            StateMachineManager::GetInstance()->GetStateMachine(*control.context().content->GetContentId())->Transition(StateEvent::RELEASE_CREATED);
        };
    }
    void newRelease(Control control, int id_content, int id_type, int id_localisation, std::string cplRefPath) {
        control.context().content->CreateRelease(id_content, id_type, id_localisation);
        changeRelease(control, std::to_string(id_content) + "_" + std::to_string(id_type) + "_" + std::to_string(id_localisation));
        control.context().release->SetReleaseInfos(cplRefPath);
        MySQLReleaseRepo* releaseRepo = new MySQLReleaseRepo();
        releaseRepo->Create(control.context().release);
        control.context().dbConnection->ExecuteQuery(releaseRepo->GetQuery());
        delete releaseRepo;
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
            changeRelease(control, Params["id_content"] + "_" + Params["id_type"] + "_" + Params["id_localisation"]);
            newCISFile(control, Params["release_cis_path"]);
            this->response.cmdStatus = "OK";
            this->response.cmdComment = "CIS uploaded";
            return this->response;
        };
        control.context().cisInteraction->pfTransitionToInProduction = [control](){
            StateMachineManager::GetInstance()->GetStateMachine(*control.context().content->GetContentId())->Transition(StateEvent::PUSH_CIS);
        };
    }
    void newCISFile(Control control, std::string cisPath) {
        control.context().release->UploadCIS(cisPath);
        MySQLCISRepo* cisRepo = new MySQLCISRepo();
        cisRepo->Create(control.context().release->GetCIS());
        control.context().dbConnection->ExecuteQuery(cisRepo->GetQuery());
        delete cisRepo;
    }
    void react(const PushCISEvent&, EventControl& control) {
        control.context().cisFinish = true;
        std::cout << "UploadCIS Finish\n";
        // if (control.context().cisFinish && control.context().syncFinish) {
        //     control.changeTo<StateInProd>();
        // }
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
            return this->response;
        };
        control.context().idleSyncInteraction->pfTransitionToSyncLoop = [control](){
            StateMachineManager::GetInstance()->GetStateMachine(*control.context().content->GetContentId())->Transition(StateEvent::CREATE_SYNC);
        };
        control.context().idleSyncInteraction->pfTransitionToCPL = [control](){
            StateMachineManager::GetInstance()->GetStateMachine(*control.context().content->GetContentId())->Transition(StateEvent::CREATE_CPL);
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
        std::cout << "CPL\n";
        control.context().cplInteraction->pfStateCPL = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
            this->response.cmdUUID = UUID;
            changeRelease(control, Params["id_content"] + "_" + Params["id_type"] + "_" + Params["id_localisation"]);
            newCPLFile(control, std::stoi(Params["id_serv_pair_config"]), Params["CPL_name"], Params["CPL_uuid"], Params["CPL_path"]);
            this->response.cmdStatus = "OK";
            this->response.cmdComment = "CPL";
            return this->response;
        };
        control.context().cplInteraction->pfTransitionToSync = [control](){
            StateMachineManager::GetInstance()->GetStateMachine(*control.context().content->GetContentId())->Transition(StateEvent::CREATE_SYNC);
        };
    }
    void newCPLFile(Control control, int id_serv_pair_config, std::string CPL_name, std::string CPL_uuid, std::string CPL_path) {
        control.context().release->UploadCPL(id_serv_pair_config, CPL_name, CPL_uuid, CPL_path);
        MySQLCPLRepo* cplRepo = new MySQLCPLRepo();
        const int* releaseId = control.context().release->GetReleaseId();
        std::string compositeId = std::to_string(id_serv_pair_config) + "_" 
                                + std::to_string(releaseId[0]) + "_" 
                                + std::to_string(releaseId[1]) + "_" 
                                + std::to_string(releaseId[2]);
        cplRepo->Create(control.context().release->GetCPL(compositeId));
        control.context().dbConnection->ExecuteQuery(cplRepo->GetQuery());
        delete cplRepo;
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
            changeRelease(control, Params["id_content"] + "_" + Params["id_type"] + "_" + Params["id_localisation"]);
            newSyncFile(control, Params["id_serv_pair_config"] + "_" + Params["id_content"] + "_" + Params["id_type"] + "_" + Params["id_localisation"], Params["path_sync"]);
            this->response.cmdStatus = "OK";
            this->response.cmdComment = "Sync";
            return this->response;
        };
        control.context().syncInteraction->pfTransitionToInProduction = [control](){
            StateMachineManager::GetInstance()->GetStateMachine(*control.context().content->GetContentId())->Transition(StateEvent::SYNC_CREATED);
        };
    }
    void newSyncFile(Control control, std::string compositeId, std::string syncPath) {
        control.context().release->GetCPL(compositeId)->CreateSync(syncPath);
        MySQLSyncRepo* syncRepo = new MySQLSyncRepo();
        syncRepo->Create(control.context().release->GetCPL(compositeId)->GetSync());
        control.context().dbConnection->ExecuteQuery(syncRepo->GetQuery());
        delete syncRepo;
    }
    void react (const SyncCreatedEvent&, EventControl& control) {
        control.context().syncFinish = true;
        std::cout << "SyncCreated Finish\n";
        // if (control.context().cisFinish && control.context().syncFinish) {
        //     control.changeTo<StateInProd>();
        // }
    }
};
 
struct StateSyncLoop : StateTemplate {
    using StateTemplate::react;
    void entryGuard(FullControl& control)  {
        control.context().syncLoopInteraction->pfStateSyncLoop = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
            this->response.cmdUUID = UUID;
            changeRelease(control, Params["id_content"] + "_" + Params["id_type"] + "_" + Params["id_localisation"]);
            newSyncLoop(control, Params["id_serv_pair_config"], Params["path_syncloop"]);
            this->response.cmdStatus = "OK";
            this->response.cmdComment = "SyncLoop";
            return this->response;
        };
        control.context().syncLoopInteraction->pfTransitionToInProduction = [control](){
            StateMachineManager::GetInstance()->GetStateMachine(*control.context().content->GetContentId())->Transition(StateEvent::SYNC_CREATED);
        };
    }
    void newSyncLoop(Control control, std::string id_serv_pair_config, std::string syncLoopPath) {
        control.context().release->UploadSyncLoop(std::stoi(id_serv_pair_config), syncLoopPath);
        MySQLSyncLoopRepo* syncLoopRepo = new MySQLSyncLoopRepo();
        const int* releaseId = control.context().release->GetReleaseId();
        std::string compositeId = id_serv_pair_config + "_" 
                                + std::to_string(releaseId[0]) + "_" 
                                + std::to_string(releaseId[1]) + "_" 
                                + std::to_string(releaseId[2]);
        syncLoopRepo->Create(control.context().release->GetSyncLoop(compositeId));
        control.context().dbConnection->ExecuteQuery(syncLoopRepo->GetQuery());
        delete syncLoopRepo;
    }
    void react (const SyncCreatedEvent&, EventControl& control) {
        control.context().syncFinish = true;
        std::cout << "SyncCreate Finish\n";
        // if (control.context().cisFinish && control.context().syncFinish) {
        //     control.changeTo<StateInProd>();
        // }  
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
    case StateEvent::CONTENT_INIT:
        this->_fsmInstance->react(ContentInitEvent{});
        break;
    case StateEvent::CREATE_CPL:
        this->_fsmInstance->react(CreateCPLEvent{});
        break;
    case StateEvent::CREATE_SYNC:
        this->_fsmInstance->react(CreateSyncEvent{});
        break;
    case StateEvent::SYNC_CREATED:
        this->_fsmInstance->react(SyncCreatedEvent{});
        break;
    case StateEvent::PUSH_CIS:
        this->_fsmInstance->react(PushCISEvent{});
        break;
    case StateEvent::PUBLISH:
        this->_fsmInstance->react(PublishEvent{});
        break;
    case StateEvent::CREATE_RELEASE:
        this->_fsmInstance->react(CreateReleaseEvent{});
        break;
    case StateEvent::RELEASE_CREATED:
        this->_fsmInstance->react(ReleaseCreatedEvent{});
        break;
    case StateEvent::CANCEL:
        this->_fsmInstance->react(CancelEvent{});
        break; 
    }
}