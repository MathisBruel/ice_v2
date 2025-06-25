#pragma once

#include "ContentOpsDomain/COD_SiteRepo.h"
#include <iostream>
#include "Query.h"
#include "ResultQuery.h"
#include "ContentOpsDomain/COD_Site.h"
#include "ContentOpsBoundary/COB_Site.h"
#include "ResultQuery.h"
#include "ContentOpsInfra/MySQLDBConnection.h"


class MySQLSiteRepo : public COD_SiteRepo
{
public:
    MySQLSiteRepo();
    virtual ~MySQLSiteRepo();
    
    void Create(COD_Site* site) override;
    void Read(COD_Site* site) override;
    void Update(COD_Site* site) override;
    void Remove(COD_Site* site) override;

    Query* MySQLcreate(COD_Site* site);
    Query* MySQLread(COD_Site* site);
    Query* MySQLread();
    Query* MySQLupdate(COD_Site* site);
    Query* MySQLremove(COD_Site* site);

    Query* GetQuery() { return _query; }

    ResultQuery* getSites();
private:
    static std::string _database;
    static std::string _table;

    int* _siteIds;
    int* _siteGroups;
    int* _siteConnections;
    std::string _siteNames;

    Query* _query;
};