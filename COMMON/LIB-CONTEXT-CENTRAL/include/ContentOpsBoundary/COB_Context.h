#pragma once
#include "ContentOpsBoundary/COB_Release.h"
#include "ContentOpsInfra/MySQLDBConnection.h"
#include "ContentOpsBoundary/COB_ReleaseRepo.h"
#include "ContentOpsBoundary/Interactions/COB_InteractionConfigurator.h"

#include <memory>

struct COB_Context {
    std::shared_ptr<COB_Release> release;  

    std::shared_ptr<bool> isNewContent;
    
    std::shared_ptr<bool> cisFinish;       
    std::shared_ptr<bool> syncFinish;      
    std::shared_ptr<int> syncCount;        
    
    int contentId = -1;                    
    int typeId = -1;                       
    int localisationId = -1;               
    
    MySQLDBConnection* dbConnection = nullptr;
    std::shared_ptr<COB_ReleaseRepo> releaseRepo;
    std::shared_ptr<COB_InteractionConfigurator> interactionConfigurator;
    
    COB_Context() = default;
    COB_Context(const COB_Context&) = default;
    
    COB_Context(int contentId, int typeId, int localisationId, MySQLDBConnection* dbConnection = nullptr)
        : contentId(contentId), typeId(typeId), localisationId(localisationId), dbConnection(dbConnection) {}
}; 