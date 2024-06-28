#include "App/Context.h"
#include "App/ContentInteraction.h"
#include "App/ReleaseInteraction.h"
#include "App/PublishingInteraction.h"
#include "App/CISInteraction.h"
#include "App/IdleSyncInteraction.h"
#include "App/CPLInteraction.h"
#include "App/SyncInteraction.h"
#include "App/SyncLoopInteraction.h"
#include "App/InProdInteraction.h"
#include "App/IdleInteraction.h"
#include "App/StateMachine.h"

#include "Infra/HTTPInteraction.h"
#include "Infra/HTTPIdleInteraction.h"
#include "Infra/HTTPCISInteraction.h"
#include "Infra/HTTPContentInteraction.h"
#include "Infra/HTTPReleaseInteraction.h"
#include "Infra/HTTPPublishingInteraction.h"
#include "Infra/HTTPIdleSyncInteraction.h"
#include "Infra/HTTPCPLInteraction.h"
#include "Infra/HTTPSyncInteraction.h"
#include "Infra/HTTPSyncLoopInteraction.h"
#include "Infra/HTTPInProdInteraction.h"

#include "commandCentral.h"

class Configurator {
public:
    Configurator();
    ~Configurator() {};

    Context* GetContext() { return this->_context; }
    StateMachine* GetStateMachine() { return this->_stateMachine; }
    std::map<CommandCentral::CommandCentralType, HTTPInteraction*> GetHTTPInteractions() { return this->_httpInteractions; }

private:
    std::map<CommandCentral::CommandCentralType, HTTPInteraction*> _httpInteractions;
    Context* _context;
    StateMachine* _stateMachine;
};