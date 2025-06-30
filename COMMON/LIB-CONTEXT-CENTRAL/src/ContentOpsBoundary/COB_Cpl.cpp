#include "ContentOpsBoundary/COB_Cpl.h"

COB_Cpl::COB_Cpl()
    : COD_Cpl()
{
}

COB_Cpl::COB_Cpl(int id, const std::string& name, const std::string& uuid, const std::string& pathSync)
    : COD_Cpl(id, name, uuid, pathSync), _id_content(-1), _id_type(-1), _id_localisation(-1)
{
}

COB_Cpl::COB_Cpl(int id, const std::string& name, const std::string& uuid, const std::string& pathSync, int id_content, int id_type, int id_localisation)
    : COD_Cpl(id, name, uuid, pathSync), _id_content(id_content), _id_type(id_type), _id_localisation(id_localisation)
{
}

COB_Cpl::~COB_Cpl()
{
}

COB_Cpl::operator std::string() const
{
    std::string xml = "<cpl";
    xml += " id=\"" + std::to_string(this->GetId()) + "\"";
    xml += " CPL_uuid=\"" + this->GetUuid() + "\"";
    xml += " CPL_name=\"" + this->GetName() + "\"";
    xml += " id_content=\"" + std::to_string(this->GetIdContent()) + "\"";
    xml += " id_type=\"" + std::to_string(this->GetIdType()) + "\"";
    xml += " id_localisation=\"" + std::to_string(this->GetIdLocalisation()) + "\"";
    xml += "/>" ;
    return xml;
} 