#include "ContentOpsBoundary/COB_Configurator.h"

COB_Configurator::COB_Configurator(MySQLDBConnection* dbConn) {
    contentInteraction = std::make_shared<COB_ContentInteraction>();
    publishingInteraction = std::make_shared<COB_PublishingInteraction>();
    releaseInteraction = std::make_shared<COB_ReleaseInteraction>();
    cisInteraction = std::make_shared<COB_CISInteraction>();
    idleSyncInteraction = std::make_shared<COB_IdleSyncInteraction>();
    cplInteraction = std::make_shared<COB_CPLInteraction>();
    syncInteraction = std::make_shared<COB_SyncInteraction>();
    syncLoopInteraction = std::make_shared<COB_SyncLoopInteraction>();
    inProdInteraction = std::make_shared<COB_InProdInteraction>();
    context = std::make_shared<COB_Context>(contentInteraction, publishingInteraction, releaseInteraction, cisInteraction, idleSyncInteraction, cplInteraction, syncInteraction, syncLoopInteraction, inProdInteraction, dbConn);
    // Remplir le mapping (exemple, à adapter selon les besoins réels)
    interactionMap[CommandCentral::CREATE_CONTENT] = contentInteraction;
    interactionMap[CommandCentral::CREATE_RELEASE] = publishingInteraction;
    interactionMap[CommandCentral::DELETE_RELEASE_CONTENT] = publishingInteraction;
    interactionMap[CommandCentral::RELEASE_CREATED] = releaseInteraction;
    interactionMap[CommandCentral::CIS_CREATED] = cisInteraction;
    interactionMap[CommandCentral::CREATE_CPL] = idleSyncInteraction;
    interactionMap[CommandCentral::CPL_CREATED] = cplInteraction;
    interactionMap[CommandCentral::SYNC_CREATED] = syncInteraction;
    interactionMap[CommandCentral::SYNCLOOP_CREATED] = syncLoopInteraction;
    interactionMap[CommandCentral::IMPORT_TO_PROD] = inProdInteraction;
} 