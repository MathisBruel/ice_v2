#pragma once
#include "COB_Context.h"
#include <map>
#include "commandCentral.h"

class COB_Configurator {
public:
    COB_Configurator(MySQLDBConnection* dbConn);
    ~COB_Configurator() = default;

    std::shared_ptr<COB_Context> getContext() { return context; }
    std::shared_ptr<COB_ContentInteraction> getContentInteraction() { return contentInteraction; }
    std::shared_ptr<COB_PublishingInteraction> getPublishingInteraction() { return publishingInteraction; }
    std::shared_ptr<COB_ReleaseInteraction> getReleaseInteraction() { return releaseInteraction; }
    std::shared_ptr<COB_CISInteraction> getCISInteraction() { return cisInteraction; }
    std::shared_ptr<COB_IdleSyncInteraction> getIdleSyncInteraction() { return idleSyncInteraction; }
    std::shared_ptr<COB_CPLInteraction> getCPLInteraction() { return cplInteraction; }
    std::shared_ptr<COB_SyncInteraction> getSyncInteraction() { return syncInteraction; }
    std::shared_ptr<COB_SyncLoopInteraction> getSyncLoopInteraction() { return syncLoopInteraction; }
    std::shared_ptr<COB_InProdInteraction> getInProdInteraction() { return inProdInteraction; }

    std::map<CommandCentral::CommandCentralType, std::shared_ptr<void>> getInteractionMap() { return interactionMap; }

private:
    std::shared_ptr<COB_Context> context;
    std::shared_ptr<COB_ContentInteraction> contentInteraction;
    std::shared_ptr<COB_PublishingInteraction> publishingInteraction;
    std::shared_ptr<COB_ReleaseInteraction> releaseInteraction;
    std::shared_ptr<COB_CISInteraction> cisInteraction;
    std::shared_ptr<COB_IdleSyncInteraction> idleSyncInteraction;
    std::shared_ptr<COB_CPLInteraction> cplInteraction;
    std::shared_ptr<COB_SyncInteraction> syncInteraction;
    std::shared_ptr<COB_SyncLoopInteraction> syncLoopInteraction;
    std::shared_ptr<COB_InProdInteraction> inProdInteraction;
    std::map<CommandCentral::CommandCentralType, std::shared_ptr<void>> interactionMap;
}; 