#pragma once

#include "ContentOpsDomain/COD_SiteRepo.h"
#include <iostream>
#include "Query.h"
#include "ResultQuery.h"
#include "ContentOpsDomain/COD_Site.h"
#include "ContentOpsBoundary/COB_Site.h"
#include "ResultQuery.h"
#include "ContentOpsInfra/MySQLDBConnection.h"
#include <memory>


class MySQLSiteRepo : public COD_SiteRepo
{
public:
    MySQLSiteRepo();
    virtual ~MySQLSiteRepo();
    
    void Create(COD_Site* site) override;
    void Read(COD_Site* site) override;
    void Update(COD_Site* site) override;
    void Remove(COD_Site* site) override;

    std::unique_ptr<Query> MySQLcreate(COD_Site* site);
    std::unique_ptr<Query> MySQLread(COD_Site* site);
    std::unique_ptr<Query> MySQLread();
    std::unique_ptr<Query> MySQLread(int* groupId);
    std::unique_ptr<Query> MySQLupdate(COD_Site* site);
    std::unique_ptr<Query> MySQLremove(COD_Site* site);

    std::unique_ptr<Query> GetQuery() { return std::move(_query); }

    std::unique_ptr<ResultQuery> getSites(int groupId) override;

private:
    static std::string _database;
    static std::string _table;

    int* _siteIds;
    int* _siteGroups;
    int* _siteConnections;
    std::string _siteNames;

    std::unique_ptr<Query> _query;
};