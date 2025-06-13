#include "App/StateMachineManager.h"

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
