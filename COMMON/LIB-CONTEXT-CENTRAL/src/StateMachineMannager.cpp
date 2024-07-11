#include "App/StateMachineManager.h"

StateMachineMannager* StateMachineMannager::_instance = nullptr;

StateMachineMannager::StateMachineMannager() { 
    _instance = nullptr;
}

StateMachineMannager* StateMachineMannager::GetInstance() {
    if(_instance == nullptr){
        _instance = new StateMachineMannager();
    }
    return _instance;
}

void StateMachineMannager::AddStateMachine(int id, StateMachine* stateMachine) {
    stateMachineMap[id] = stateMachine;
}

StateMachine* StateMachineMannager::GetStateMachine(int id) {
    return stateMachineMap[id];
};