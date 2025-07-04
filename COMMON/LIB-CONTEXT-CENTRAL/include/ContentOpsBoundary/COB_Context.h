#pragma once
#include "ContentOpsBoundary/COB_Content.h"
#include "ContentOpsBoundary/COB_Release.h"
#include "ContentOpsInfra/MySQLDBConnection.h"
#include "ContentOpsBoundary/COB_ContentRepo.h"
#include "ContentOpsBoundary/COB_ReleaseRepo.h" 
#include "ContentOpsBoundary/Interactions/COB_InteractionConfigurator.h"

#include <memory>

struct COB_Context {
    bool cisFinish = false;
    bool syncFinish = false;
    std::shared_ptr<COB_Content> content;
    std::shared_ptr<COB_Release> release;
    MySQLDBConnection* dbConnection = nullptr;
    COB_Context() = default;
    COB_Context(const COB_Context&) = default;
    COB_Context(MySQLDBConnection* dbConnection)
        : dbConnection(dbConnection) {}
    std::shared_ptr<COB_ContentRepo> contentRepo;
    std::shared_ptr<COB_ReleaseRepo> releaseRepo;
    std::shared_ptr<bool> isNewContent;
    std::shared_ptr<std::string> pendingTitle;
    std::shared_ptr<COB_InteractionConfigurator> interactionConfigurator;
}; 