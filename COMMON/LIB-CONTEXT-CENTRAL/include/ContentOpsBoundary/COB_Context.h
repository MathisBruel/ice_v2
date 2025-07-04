#pragma once
#include "ContentOpsBoundary/Interactions/COB_ContentInteraction.h"
#include "ContentOpsBoundary/Interactions/COB_PublishingInteraction.h"
#include "ContentOpsBoundary/Interactions/COB_ReleaseInteraction.h"
#include "ContentOpsBoundary/Interactions/COB_CISInteraction.h"
#include "ContentOpsBoundary/Interactions/COB_IdleSyncInteraction.h"
#include "ContentOpsBoundary/Interactions/COB_CPLInteraction.h"
#include "ContentOpsBoundary/Interactions/COB_SyncInteraction.h"
#include "ContentOpsBoundary/Interactions/COB_SyncLoopInteraction.h"
#include "ContentOpsBoundary/Interactions/COB_InProdInteraction.h"
#include "ContentOpsBoundary/COB_Content.h"
#include "ContentOpsBoundary/COB_Release.h"
#include "ContentOpsInfra/MySQLDBConnection.h"
#include "ContentOpsBoundary/COB_ContentRepo.h"
#include "ContentOpsBoundary/COB_ReleaseRepo.h" 

#include <memory>

struct COB_Context {
    bool cisFinish = false;
    bool syncFinish = false;
    std::shared_ptr<COB_Content> content;
    std::shared_ptr<COB_Release> release;
    std::shared_ptr<COB_ContentInteraction> contentInteraction;
    std::shared_ptr<COB_PublishingInteraction> publishingInteraction;
    std::shared_ptr<COB_ReleaseInteraction> releaseInteraction;
    std::shared_ptr<COB_CISInteraction> cisInteraction;
    std::shared_ptr<COB_IdleSyncInteraction> idleSyncInteraction;
    std::shared_ptr<COB_CPLInteraction> cplInteraction;
    std::shared_ptr<COB_SyncInteraction> syncInteraction;
    std::shared_ptr<COB_SyncLoopInteraction> syncLoopInteraction;
    std::shared_ptr<COB_InProdInteraction> inProdInteraction;
    MySQLDBConnection* dbConnection = nullptr;
    COB_Context() = default;
    COB_Context(const COB_Context&) = default;
    COB_Context(std::shared_ptr<COB_ContentInteraction> contentInteraction,
                std::shared_ptr<COB_PublishingInteraction> publishingInteraction,
                std::shared_ptr<COB_ReleaseInteraction> releaseInteraction,
                std::shared_ptr<COB_CISInteraction> cisInteraction,
                std::shared_ptr<COB_IdleSyncInteraction> idleSyncInteraction,
                std::shared_ptr<COB_CPLInteraction> cplInteraction,
                std::shared_ptr<COB_SyncInteraction> syncInteraction,
                std::shared_ptr<COB_SyncLoopInteraction> syncLoopInteraction,
                std::shared_ptr<COB_InProdInteraction> inProdInteraction,
                MySQLDBConnection* dbConnection)
        : contentInteraction(contentInteraction),
          publishingInteraction(publishingInteraction),
          releaseInteraction(releaseInteraction),
          cisInteraction(cisInteraction),
          idleSyncInteraction(idleSyncInteraction),
          cplInteraction(cplInteraction),
          syncInteraction(syncInteraction),
          syncLoopInteraction(syncLoopInteraction),
          inProdInteraction(inProdInteraction),
          dbConnection(dbConnection) {}
    std::shared_ptr<COB_ContentRepo> contentRepo;
    std::shared_ptr<COB_ReleaseRepo> releaseRepo;
    std::shared_ptr<bool> isNewContent;
    std::shared_ptr<std::string> pendingTitle;
}; 