#include "App/Context.h"
#include "App/StateMachine.h"
#include "App/StateMachine.h"
 
#include "Infra/HTTPInteraction.h"
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
 
#include "commandCentral.h"
 
class Configurator {
public:
    Configurator(MySQLDBConnection* DBconnection);
    ~Configurator() {};
    
    FSM::Instance * fsmMachine = nullptr;
    std::map<CommandCentral::CommandCentralType, HTTPInteraction*> GetHTTPInteractions() { return this->_httpInteractions; }
    StateMachine* GetStateMachine() { return this->_stateMachine; }
private:
    std::map<CommandCentral::CommandCentralType, HTTPInteraction*> _httpInteractions;
    Context* _context;
    StateMachine* _stateMachine;
    MySQLDBConnection* _dbConnection;
};
 