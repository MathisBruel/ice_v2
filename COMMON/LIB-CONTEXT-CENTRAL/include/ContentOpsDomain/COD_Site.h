#pragma once

#include <string>

class COD_SiteRepo;

class COD_Site
{
public:
    COD_Site();
    ~COD_Site();

    void SetSiteId(int siteId);
    void SetDatas(std::string siteName, int siteGroup, int siteConnection);

    // Versions const pour toXmlString()
    int GetSiteId() const {return this->_siteId;}
    std::string GetSiteName() const {return this->_siteName;}
    int GetSiteGroup() const {return this->_siteGroup;}
    int GetSiteConnection() const {return this->_siteConnection;}

    // Versions non-const pour MySQLSiteRepo (compatibilité)
    int* GetSiteIdPtr() {return &this->_siteId;}
    int& GetSiteGroupRef() {return this->_siteGroup;}
    int& GetSiteConnectionRef() {return this->_siteConnection;}

private:
    COD_SiteRepo* _siteRepo;
    int _siteId;
    std::string _siteName;
    int _siteGroup;
    int _siteConnection;
};