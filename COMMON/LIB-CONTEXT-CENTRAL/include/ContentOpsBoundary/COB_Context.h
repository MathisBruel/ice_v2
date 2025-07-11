#pragma once
#include "ContentOpsBoundary/COB_Release.h"
#include "ContentOpsInfra/MySQLDBConnection.h"
#include "ContentOpsBoundary/COB_ReleaseRepo.h"
#include "ContentOpsBoundary/COB_SyncLoopRepo.h"
#include "ContentOpsBoundary/Interactions/COB_InteractionConfigurator.h"

#include <memory>

struct COB_Context {
    std::shared_ptr<COB_Release> release;  // Release gérée par cette statemachine
    
    std::shared_ptr<bool> cisFinish;       // Flag pour la branche CIS
    std::shared_ptr<bool> syncFinish;      // Flag pour la branche Sync
    std::shared_ptr<int> syncCount;        // Compteur de syncs reçus
    
    int contentId = -1;                    // ID du contenu parent
    int typeId = -1;                       // Type de la release
    int localisationId = -1;               // Localisation de la release
    
    MySQLDBConnection* dbConnection = nullptr;
    std::shared_ptr<COB_ReleaseRepo> releaseRepo;
    std::shared_ptr<COB_SyncLoopRepo> syncLoopRepo;
    std::shared_ptr<COB_InteractionConfigurator> interactionConfigurator;
    
    COB_Context() = default;
    COB_Context(const COB_Context&) = default;
    
    COB_Context(int contentId, int typeId, int localisationId, MySQLDBConnection* dbConnection = nullptr)
        : contentId(contentId), typeId(typeId), localisationId(localisationId), dbConnection(dbConnection) {}
}; 