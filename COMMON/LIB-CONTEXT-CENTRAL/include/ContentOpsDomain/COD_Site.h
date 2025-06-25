#pragma once

#include "ContentOpsDomain/COD_SiteRepo.h"

class COD_Site
{
public:
    COD_Site();
    ~COD_Site();

    void SetSiteId(int siteId);
    void SetDatas(std::string siteName, int siteGroup, int siteConnection);

    int* GetSiteId() {return &this->_siteId;}
    std::string GetSiteName() {return this->_siteName;}
    int GetSiteGroup() {return this->_siteGroup;}
    int GetSiteConnection() {return this->_siteConnection;}

private:
    COD_SiteRepo* _siteRepo;
    int _siteId;
    std::string _siteName;
    int _siteGroup;
    int _siteConnection;
};