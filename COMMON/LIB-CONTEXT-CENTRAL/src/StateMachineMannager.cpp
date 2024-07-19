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
    _stateMachineMap[id] = stateMachine;
}

StateMachine* StateMachineManager::GetStateMachine(int id) {
    return _stateMachineMap[id];
};