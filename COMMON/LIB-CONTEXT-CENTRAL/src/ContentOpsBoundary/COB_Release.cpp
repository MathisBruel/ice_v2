#include "ContentOpsBoundary/COB_Release.h"

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
    if (!this->GetTypeName().empty()) {
        xml += " type_name=\"" + this->GetTypeName() + "\"";
    }
    if (!this->GetLocalisationName().empty()) {
        xml += " localisation_name=\"" + this->GetLocalisationName() + "\"";
    }
    
    xml += "/>";
    return xml;
}