#pragma once
#include <map>

#include "ContentOpsApp/Context.h"

#include "ContentOpsInfra/HTTPContentInteraction.h"
#include "ContentOpsInfra/HTTPPublishingInteraction.h"
#include "ContentOpsInfra/HTTPReleaseInteraction.h"
#include "ContentOpsInfra/HTTPCISInteraction.h"
#include "ContentOpsInfra/HTTPIdleSyncInteraction.h"
#include "ContentOpsInfra/HTTPCPLInteraction.h"
#include "ContentOpsInfra/HTTPSyncInteraction.h"
#include "ContentOpsInfra/HTTPSyncLoopInteraction.h"
#include "ContentOpsInfra/HTTPInProdInteraction.h"
#include "ContentOpsInfra/MySQLDBConnection.h"

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