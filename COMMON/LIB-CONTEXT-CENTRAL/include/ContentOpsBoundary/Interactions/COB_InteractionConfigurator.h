#pragma once
#include "COB_Interaction.h"
#include "COB_ReleaseInteraction.h"
#include "COB_CPLInteraction.h"
#include "COB_InProdInteraction.h"
#include "COB_UploadInteraction.h"  
#include "COB_ArchiveInteraction.h"
#include "commandCentral.h"

class COB_InteractionConfigurator {
public:
    COB_InteractionConfigurator();
    ~COB_InteractionConfigurator();
    std::map<CommandCentral::CommandCentralType, COB_Interaction*> GetInteractions() { return this->_interactions; }
private:
    std::map<CommandCentral::CommandCentralType, COB_Interaction*> _interactions;
};