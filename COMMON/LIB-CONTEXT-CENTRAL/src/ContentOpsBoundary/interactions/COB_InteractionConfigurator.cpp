#include "ContentOpsBoundary/Interactions/COB_InteractionConfigurator.h"

COB_InteractionConfigurator::COB_InteractionConfigurator() {
    
    COB_ReleaseInteraction* pfReleaseInteraction = new COB_ReleaseInteraction();
    COB_UploadInteraction* pfUploadInteraction = new COB_UploadInteraction();
    COB_CPLInteraction* pfCPLInteraction = new COB_CPLInteraction();
    COB_InProdInteraction* pfInProdInteraction = new COB_InProdInteraction();
    
    COB_ArchiveInteraction* pfArchiveInteraction = new COB_ArchiveInteraction();

    this->_interactions[CommandCentral::CREATE_RELEASE] = pfReleaseInteraction;
    this->_interactions[CommandCentral::RELEASE_CREATED] = pfReleaseInteraction;
    this->_interactions[CommandCentral::CIS_CREATED] = pfUploadInteraction;
    this->_interactions[CommandCentral::SYNC_CREATED] = pfUploadInteraction;
    this->_interactions[CommandCentral::SYNCLOOP_CREATED] = pfUploadInteraction;
    this->_interactions[CommandCentral::CPL_CREATED] = pfCPLInteraction;
    this->_interactions[CommandCentral::IMPORT_TO_PROD] = pfInProdInteraction;
    this->_interactions[CommandCentral::ARCHIVE_RELEASE] = pfArchiveInteraction;
}

COB_InteractionConfigurator::~COB_InteractionConfigurator() {
    for (auto& pair : _interactions) {
        delete pair.second;
    }
    _interactions.clear();
}