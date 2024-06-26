#include "App/Context.hpp"
#include "App/Content_Interaction.h"
#include "App/Release_Interaction.h"
#include "App/Publishing_Interaction.h"
#include "App/CIS_Interaction.h"
#include "App/IdleSync_Interaction.h"
#include "App/CPL_Interaction.h"
#include "App/Sync_Interaction.h"
#include "App/SyncLoop_Interaction.h"
#include "App/InProd_Interaction.h"
#include "App/Idle_Interaction.h"
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

    Context* getContext() { return this->context; }
    StateMachine* getStateMachine() { return this->stateMachine; }
    std::map<CommandCentral::CommandCentralType, HTTPInteraction*> getHTTPInteractions() { return this->httpInteractions; }

private:
    std::map<CommandCentral::CommandCentralType, HTTPInteraction*> httpInteractions;
    Context* context;
    StateMachine* stateMachine;
};