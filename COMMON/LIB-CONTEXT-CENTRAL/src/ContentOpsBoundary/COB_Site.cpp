#include "ContentOpsBoundary/COB_Site.h"
#include <string>

COB_Site::COB_Site(int siteId, std::string siteName, int siteGroup, int siteConnection)
    : COD_Site(siteId, siteName, siteGroup, siteConnection)
{
}

COB_Site::operator std::string() const
{
    std::string xml = "<site";
    xml += " id_site=\"" + std::to_string(this->GetSiteId()) + "\"";
    xml += " id_group=\"" + std::to_string(this->GetSiteGroup()) + "\"";
    xml += " id_connection=\"" + std::to_string(this->GetSiteConnection()) + "\"";
    xml += " name=\"" + this->GetSiteName() + "\"";
    xml += "/>";
    return xml;
}
