#pragma once

#include <string>
#include <memory>

class COD_SiteRepo;

class COD_Site
{
public:
    COD_Site(int siteId, std::string siteName, int siteGroup, int siteConnection);
    ~COD_Site();

    void SetSiteId(int siteId);
    void SetDatas(std::string siteName, int siteGroup, int siteConnection);

    int GetSiteId() const {return this->_siteId;}
    std::string GetSiteName() const {return this->_siteName;}
    int GetSiteGroup() const {return this->_siteGroup;}
    int GetSiteConnection() const {return this->_siteConnection;}

    int* GetSiteIdPtr() {return &this->_siteId;}
    int& GetSiteGroupRef() {return this->_siteGroup;}
    int& GetSiteConnectionRef() {return this->_siteConnection;}

private:
    std::shared_ptr<COD_SiteRepo> _siteRepo;
    int _siteId;
    std::string _siteName;
    int _siteGroup;
    int _siteConnection;
};