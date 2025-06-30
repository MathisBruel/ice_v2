#include "ContentOpsBoundary/COB_Localisation.h"

COB_Localisation::COB_Localisation()
    : COD_Localisation()
{
}

COB_Localisation::COB_Localisation(int id, std::string name)
    : COD_Localisation(id, name)
{
}

COB_Localisation::~COB_Localisation()
{
}

COB_Localisation::operator std::string() const
{
    std::string xml = "<localisation";
    xml += " id=\"" + std::to_string(this->GetId()) + "\"";
    xml += " name=\"" + this->GetName() + "\"";
    xml += "/>";
    return xml;
} 