#include "ContentOpsDomain/COD_Site.h"

COD_Site::COD_Site()
{
    this->_siteId = -1;
    this->_siteName = "";
    this->_siteGroup = -1;
    this->_siteConnection = -1;
}
COD_Site::~COD_Site()
{
    if (this->_siteRepo != nullptr)
    {
        delete this->_siteRepo;
    }
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



