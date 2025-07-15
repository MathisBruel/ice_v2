#include "ContentOpsBoundary/COB_Release.h"
#include "ContentOpsBoundary/BoundaryManager.h"

COB_Release::COB_Release(int id_content, int typeMovie, int localisationMovie)
    : COD_Releases(id_content, typeMovie, localisationMovie)
{
}

COB_Release::operator std::string() const
{
    std::string xml = "<release";
    xml += " id_content=\"" + std::to_string(this->GetReleaseId()[0]) + "\"";
    xml += " id_type=\"" + std::to_string(this->GetReleaseId()[1]) + "\"";
    xml += " id_localisation=\"" + std::to_string(this->GetReleaseId()[2]) + "\"";
    xml += " release_cpl_ref_path=\"" + this->GetCPLRefPath() + "\"";
    xml += " release_cis_path=\"" + this->GetCISPath() + "\"";
    
    // Ajouter l'état de la release via BoundaryManager
    try {
        std::string releaseState = BoundaryManager::GetInstance().GetReleaseState(
            this->GetReleaseId()[0], 
            this->GetReleaseId()[1], 
            this->GetReleaseId()[2]
        );
        xml += " state=\"" + releaseState + "\"";
    } catch (const std::exception& e) {
        xml += " state=\"UNKNOWN\"";
    }
    
    if (!this->GetTypeName().empty()) {
        xml += " type_name=\"" + this->GetTypeName() + "\"";
    }
    if (!this->GetLocalisationName().empty()) {
        xml += " localisation_name=\"" + this->GetLocalisationName() + "\"";
    }
    if (!this->GetSyncLoopPath().empty()) {
        xml += " release_syncloop_path=\"" + this->GetSyncLoopPath() + "\"";
    }
    
    xml += "/>";
    return xml;
}