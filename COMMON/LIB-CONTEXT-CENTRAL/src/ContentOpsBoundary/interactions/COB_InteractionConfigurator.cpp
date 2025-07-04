#include "ContentOpsBoundary/Interactions/COB_InteractionConfigurator.h"

COB_InteractionConfigurator::COB_InteractionConfigurator() {
    COB_ContentInteraction* pfContentInteraction = new COB_ContentInteraction();
    COB_PublishingInteraction* pfPublishingInteraction = new COB_PublishingInteraction();
    COB_ReleaseInteraction* pfReleaseInteraction = new COB_ReleaseInteraction();
    COB_CISInteraction* pfCISInteraction = new COB_CISInteraction();
    COB_IdleSyncInteraction* pfIdleSyncInteraction = new COB_IdleSyncInteraction();
    COB_CPLInteraction* pfCPLInteraction = new COB_CPLInteraction();
    COB_SyncInteraction* pfSyncInteraction = new COB_SyncInteraction();
    COB_SyncLoopInteraction* pfSyncLoopInteraction = new COB_SyncLoopInteraction();
    COB_InProdInteraction* pfInProdInteraction = new COB_InProdInteraction();

    this->_interactions[CommandCentral::CREATE_CONTENT] = pfContentInteraction;
    this->_interactions[CommandCentral::CREATE_RELEASE] = pfPublishingInteraction;
    this->_interactions[CommandCentral::DELETE_RELEASE_CONTENT] = pfPublishingInteraction;
    this->_interactions[CommandCentral::DELETE_RELEASE_CPL] = pfPublishingInteraction;
    this->_interactions[CommandCentral::DELETE_RELEASE_SYNC] = pfPublishingInteraction;
    this->_interactions[CommandCentral::DELETE_RELEASE_SYNCLOOP] = pfPublishingInteraction;
    this->_interactions[CommandCentral::RELEASE_CREATED] = pfReleaseInteraction;
    this->_interactions[CommandCentral::CIS_CREATED] = pfCISInteraction;
    this->_interactions[CommandCentral::CREATE_CPL] = pfIdleSyncInteraction;
    this->_interactions[CommandCentral::CREATE_SYNCLOOP] = pfIdleSyncInteraction;
    this->_interactions[CommandCentral::CPL_CREATED] = pfCPLInteraction;
    this->_interactions[CommandCentral::SYNC_CREATED] = pfSyncInteraction;
    this->_interactions[CommandCentral::SYNCLOOP_CREATED] = pfSyncLoopInteraction;
    this->_interactions[CommandCentral::IMPORT_TO_PROD] = pfInProdInteraction;
}

COB_InteractionConfigurator::~COB_InteractionConfigurator() {
    // Clean up allocated interactions
    for (auto& pair : _interactions) {
        delete pair.second;
    }
    _interactions.clear();
}