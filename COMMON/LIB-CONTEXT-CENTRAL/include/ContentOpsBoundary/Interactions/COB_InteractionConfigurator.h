#pragma once
#include "COB_Interaction.h"
#include "COB_ContentInteraction.h"
#include "COB_PublishingInteraction.h"
#include "COB_ReleaseInteraction.h"
#include "COB_CISInteraction.h"
#include "COB_IdleSyncInteraction.h"
#include "COB_CPLInteraction.h"
#include "COB_SyncInteraction.h"
#include "COB_SyncLoopInteraction.h"
#include "COB_InProdInteraction.h"
#include "commandCentral.h"

class COB_InteractionConfigurator {
public:
    COB_InteractionConfigurator();
    ~COB_InteractionConfigurator();
    std::map<CommandCentral::CommandCentralType, COB_Interaction*> GetInteractions() { return this->_interactions; }
private:
    std::map<CommandCentral::CommandCentralType, COB_Interaction*> _interactions;
};