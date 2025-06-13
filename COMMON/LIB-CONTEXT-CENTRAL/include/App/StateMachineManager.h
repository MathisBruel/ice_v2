#pragma once
#include <map>

#include "App/Context.h"

#include "Infra/HTTPContentInteraction.h"
#include "Infra/HTTPPublishingInteraction.h"
#include "Infra/HTTPReleaseInteraction.h"
#include "Infra/HTTPCISInteraction.h"
#include "Infra/HTTPIdleSyncInteraction.h"
#include "Infra/HTTPCPLInteraction.h"
#include "Infra/HTTPSyncInteraction.h"
#include "Infra/HTTPSyncLoopInteraction.h"
#include "Infra/HTTPInProdInteraction.h"
#include "Infra/MySQLDBConnection.h"

class StateMachine; 

class StateMachineManager {
    public:
        static StateMachineManager* GetInstance();

        void AddStateMachine(int id, StateMachine* stateMachine);
        StateMachine* GetStateMachine(int id);
        StateMachine* CreateStateMachine(int contentId, MySQLDBConnection* dbConnection);
    private:
        StateMachineManager();
        static StateMachineManager* _instance;
        std::map<int,StateMachine*> _stateMachineMap;
};