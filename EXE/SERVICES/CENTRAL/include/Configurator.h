#include "ContentOpsApp/Context.h"
#include "ContentOpsApp/StateMachine.h"
#include "ContentOpsApp/StateMachine.h"
 
#include "ContentOpsInfra/HTTPInteraction.h"
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
 
#include "commandCentral.h"
 
//class Configurator {
//public:
//    Configurator(MySQLDBConnection* DBconnection);
//    ~Configurator() {};
//    
//    FSM::Instance * fsmMachine = nullptr;
//    std::map<CommandCentral::CommandCentralType, HTTPInteraction*> GetHTTPInteractions() { return this->_httpInteractions; }
//    StateMachine* GetStateMachine() { return this->_stateMachine; }
//private:
//    std::map<CommandCentral::CommandCentralType, HTTPInteraction*> _httpInteractions;
//    Context* _context;
//    StateMachine* _stateMachine;
//    MySQLDBConnection* _dbConnection;
//};
 