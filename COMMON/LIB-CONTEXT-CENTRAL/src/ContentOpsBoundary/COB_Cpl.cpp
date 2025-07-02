#include "ContentOpsBoundary/COB_Cpl.h"

COB_Cpl::COB_Cpl()
    : COD_Cpl()
{
}

COB_Cpl::COB_Cpl(int id, const std::string& name, const std::string& uuid, const std::string& pathSync, int type_cpl, const std::string& sha1_sync, const std::string& path_cpl, long long id_serv_pair_config, long long id_content, long long id_type, long long id_localisation)
    : COD_Cpl(id, name, uuid, pathSync)
{
    SetTypeCpl(type_cpl);
    SetSha1Sync(sha1_sync);
    SetPathCpl(path_cpl);
    SetIdServPairConfig(id_serv_pair_config);
    SetIdContent(id_content);
    SetIdType(id_type);
    SetIdLocalisation(id_localisation);
}

COB_Cpl::~COB_Cpl()
{
}

COB_Cpl::operator std::string() const
{
    std::string xml = "<cpl";
    xml += " id=\"" + std::to_string(this->GetId()) + "\"";
    xml += " uuid=\"" + this->GetUuid() + "\"";
    xml += " name=\"" + this->GetName() + "\"";
    xml += " type_cpl=\"" + std::to_string(this->GetTypeCpl()) + "\"";
    xml += " sha1_sync=\"" + this->GetSha1Sync() + "\"";
    xml += " path_cpl=\"" + this->GetPathCpl() + "\"";
    xml += " path_sync=\"" + this->GetPathSync() + "\"";
    xml += " id_serv_pair_config=\"" + std::to_string(this->GetIdServPairConfig()) + "\"";
    xml += " id_content=\"" + std::to_string(this->GetIdContent()) + "\"";
    xml += " id_type=\"" + std::to_string(this->GetIdType()) + "\"";
    xml += " id_localisation=\"" + std::to_string(this->GetIdLocalisation()) + "\"";
    xml += "/>";
    return xml;
} 