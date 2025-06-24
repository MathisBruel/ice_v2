#pragma once

class COD_Site
{
public:
    COD_Site();
    ~COD_Site();

    void SetSiteId(int siteId);
    void SetSiteName(std::string siteName);
    void SetSiteGroup(int siteGroup);
    void SetSiteConnection(int siteConnection);

    int* GetSiteId() {return &this->_siteId;}
    std::string GetSiteName() {return this->_siteName;}
    int GetSiteGroup() {return this->_siteGroup;}
    int GetSiteConnection() {return this->_siteConnection;}

private:
    SiteRepo* _siteRepo;
    int _siteId;
    std::string _siteName;
    int _siteGroup;
    int _siteConnection;
};