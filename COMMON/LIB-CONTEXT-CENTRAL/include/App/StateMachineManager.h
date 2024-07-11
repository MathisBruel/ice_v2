#include "StateMachine.h"
class StateMachineMannager {
    public:
        static StateMachineMannager* GetInstance();

        void AddStateMachine(int id, StateMachine* stateMachine);
        StateMachine* GetStateMachine(int id);
    private:
        StateMachineMannager();
        static StateMachineMannager* _instance;
        std::map<int,StateMachine*> stateMachineMap;
};