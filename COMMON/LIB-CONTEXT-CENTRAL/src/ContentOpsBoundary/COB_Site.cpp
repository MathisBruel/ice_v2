#include "ContentOpsBoundary/COB_Site.h"

std::string COB_Site::toXmlString(bool printChild)
{
    std::string xml = "<site";
    xml += " id_site=\"" + std::to_string(*this->GetSiteId()) + "\"";
    xml += " id_group=\"" + std::to_string(this->GetSiteGroup()) + "\"";
    xml += " id_connection=\"" + std::to_string(this->GetSiteConnection()) + "\"";
    xml += " name=\"" + this->GetSiteName() + "\"";
    xml += "/>";
    return xml;
}
