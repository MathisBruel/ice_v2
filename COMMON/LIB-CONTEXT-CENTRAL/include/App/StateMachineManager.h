#pragma once
class StateMachine; 

#include <map>

class StateMachineManager {
    public:
        static StateMachineManager* GetInstance();

        void AddStateMachine(int id, StateMachine* stateMachine);
        StateMachine* GetStateMachine(int id);
        void RemoveStateMachine(int id);
    private:
        StateMachineManager();
        static StateMachineManager* _instance;
        std::map<int,StateMachine*> _stateMachineMap;
};