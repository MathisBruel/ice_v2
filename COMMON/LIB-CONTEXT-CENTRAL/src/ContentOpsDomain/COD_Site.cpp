#include "ContentOpsDomain/COD_Site.h"

COD_Site::COD_Site(int siteId, std::string siteName, int siteGroup, int siteConnection)
{
    this->_siteId = siteId;
    this->_siteName = siteName;
    this->_siteGroup = siteGroup;
    this->_siteConnection = siteConnection;
}
COD_Site::~COD_Site()
{
}



void COD_Site::SetSiteId(int siteId)
{
    this->_siteId = siteId;
}
void COD_Site::SetDatas(std::string siteName, int siteGroup, int siteConnection)
{
    this->_siteName = siteName;
    this->_siteGroup = siteGroup;
    this->_siteConnection = siteConnection;
}



