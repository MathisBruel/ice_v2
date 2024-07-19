#include "StateMachine.h"
class StateMachineManager {
    public:
        static StateMachineManager* GetInstance();

        void AddStateMachine(int id, StateMachine* stateMachine);
        StateMachine* GetStateMachine(int id);
    private:
        StateMachineManager();
        static StateMachineManager* _instance;
        std::map<int,StateMachine*> _stateMachineMap;
};