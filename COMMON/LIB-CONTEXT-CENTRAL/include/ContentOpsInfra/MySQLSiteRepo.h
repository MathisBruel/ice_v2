#pragma once

#include "ContentOpsDomain/COD_SiteRepo.h"

class MySQLSiteRepo : public COD_SiteRepo
{
public: 
    void Create(COD_Site* site) override { _query = MySQLcreate(site); }
    void Read(COD_Site* site) override { _query = MySQLread(site); }
    void Update(COD_Site* site) override { _query = MySQLupdate(site); }
    void Remove(COD_Site* site) override { _query = MySQLremove(site); }

    Query* MySQLcreate(COD_Site* site);
    Query* MySQLread(COD_Site* site);
    Query* MySQLread();
    Query* MySQLupdate(COD_Site* site);
    Query* MySQLremove(COD_Site* site);

    Query* GetQuery() { return _query; }

private:
    static std::string _database;
    static std::string _table;

    int* _siteIds;
    int* _siteGroups;
    int* _siteConnections;
    std::string _siteNames;

    Query* _query;
};