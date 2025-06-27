#include "ContentOpsApp//StateMachineManager.h"
#include "ContentOpsApp//StateMachine.h"

StateMachineManager* StateMachineManager::_instance = nullptr;

StateMachineManager::StateMachineManager() { 
    _instance = nullptr;
}

StateMachineManager* StateMachineManager::GetInstance() {
    if(_instance == nullptr){
        _instance = new StateMachineManager();
    }
    return _instance;
}

void StateMachineManager::AddStateMachine(int id, StateMachine* stateMachine) {
    if (_stateMachineMap.find(id) != _stateMachineMap.end()) {
        delete _stateMachineMap[id];
    }
    _stateMachineMap[id] = stateMachine;
}

StateMachine* StateMachineManager::GetStateMachine(int id) {
    if (_stateMachineMap.find(id) == _stateMachineMap.end()) {
        return nullptr;
    }
    return _stateMachineMap[id];
}


StateMachine* StateMachineManager::CreateStateMachine(int contentId, MySQLDBConnection* dbConnection) {
    // Création d'un nouveau contexte pour le Content
    Context* contentContext = new Context(
        new HTTPContentInteraction(),
        new HTTPPublishingInteraction(),
        new HTTPReleaseInteraction(),
        new HTTPCISInteraction(),
        new HTTPIdleSyncInteraction(),
        new HTTPCPLInteraction(),
        new HTTPSyncInteraction(),
        new HTTPSyncLoopInteraction(),
        new HTTPInProdInteraction(),
        dbConnection
    );

    StateMachine* contentStateMachine = new StateMachine(contentContext);    
    StateMachineManager::GetInstance()->AddStateMachine(contentId, contentStateMachine);
    
    return contentStateMachine;
}