#include "ContentOpsBoundary/COB_Type.h"

COB_Type::COB_Type()
    : COD_Type()
{
}

COB_Type::COB_Type(int id, std::string name)
    : COD_Type(id, name)
{
}

COB_Type::~COB_Type()
{
}

COB_Type::operator std::string() const
{
    std::string xml = "<type";
    xml += " id=\"" + std::to_string(this->GetId()) + "\"";
    xml += " name=\"" + this->GetName() + "\"";
    xml += "/>";
    return xml;
} 