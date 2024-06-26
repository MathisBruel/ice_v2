#include "Configurator.h"

Configurator::Configurator() {

    Idle_Interaction* pfIdle_Interaction = new Idle_Interaction();
    Content_Interaction* pfContent_Interaction = new Content_Interaction();
    Release_Interaction* pfRelease_Interaction = new Release_Interaction();
    Publishing_Interaction* pfPublishing_Interaction = new Publishing_Interaction();
    CIS_Interaction* pfCIS_Interaction = new CIS_Interaction();
    IdleSync_Interaction* pfIdleSync_Interaction = new IdleSync_Interaction();
    CPL_Interaction* pfCPL_Interaction = new CPL_Interaction();
    Sync_Interaction* pfSync_Interaction = new Sync_Interaction();
    SyncLoop_Interaction* pfSyncLoop_Interaction = new SyncLoop_Interaction();
    InProd_Interaction* pfInProd_Interaction = new InProd_Interaction();
    this->context = new Context(pfIdle_Interaction, 
                                pfContent_Interaction, 
                                pfRelease_Interaction, 
                                pfPublishing_Interaction, 
                                pfCIS_Interaction, 
                                pfIdleSync_Interaction, 
                                pfCPL_Interaction, 
                                pfSync_Interaction, 
                                pfSyncLoop_Interaction, 
                                pfInProd_Interaction);
    this->stateMachine = new StateMachine(*this->context);
    this->httpInteractions [CommandCentral::CREATE_CONTENT] = new HTTPIdleInteraction(pfIdle_Interaction);
    this->httpInteractions [CommandCentral::CONTENT_CREATED] = new HTTPContentInteraction(pfContent_Interaction);
    this->httpInteractions [CommandCentral::RELEASE_CREATED] = new HTTPReleaseInteraction(pfRelease_Interaction);
    this->httpInteractions [CommandCentral::CREATE_RELEASE] = new HTTPPublishingInteraction(pfPublishing_Interaction);
    this->httpInteractions [CommandCentral::CIS_CREATED] = new HTTPCISInteraction(pfCIS_Interaction);
    this->httpInteractions [CommandCentral::CREATE_CPL] = new HTTPIdleSyncInteraction(pfIdleSync_Interaction, false);
    this->httpInteractions [CommandCentral::CREATE_SYNCLOOP] = new HTTPIdleSyncInteraction(pfIdleSync_Interaction, true);
    this->httpInteractions [CommandCentral::CPL_CREATED] = new HTTPCPLInteraction(pfCPL_Interaction);
    this->httpInteractions [CommandCentral::SYNC_CREATED] = new HTTPSyncInteraction(pfSync_Interaction);
    this->httpInteractions [CommandCentral::SYNCLOOP_CREATED] = new HTTPSyncLoopInteraction(pfSyncLoop_Interaction);
    this->httpInteractions [CommandCentral::IMPORT_TO_PROD] = new HTTPInProdInteraction(pfInProd_Interaction);
}