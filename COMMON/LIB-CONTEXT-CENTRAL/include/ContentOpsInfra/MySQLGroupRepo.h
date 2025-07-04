#pragma once

#include "ContentOpsDomain/COD_GroupRepo.h"
#include <iostream>
#include "Query.h"
#include "ResultQuery.h"
#include "ContentOpsDomain/COD_Group.h"
#include "ContentOpsBoundary/COB_Group.h"
#include "ResultQuery.h"
#include "ContentOpsInfra/MySQLDBConnection.h"
#include <memory>

class MySQLGroupRepo : public COD_GroupRepo
{
public:
    MySQLGroupRepo();
    MySQLGroupRepo(std::shared_ptr<MySQLDBConnection> dbConnection);
    virtual ~MySQLGroupRepo();
    
    void Create(COD_Group* group) override;
    void Read(COD_Group* group) override;
    void Update(COD_Group* group) override;
    void Remove(COD_Group* group) override;

    std::unique_ptr<Query> MySQLcreate(COD_Group* group);
    std::unique_ptr<Query> MySQLread(COD_Group* group);
    std::unique_ptr<Query> MySQLread();
    std::unique_ptr<Query> MySQLupdate(COD_Group* group);
    std::unique_ptr<Query> MySQLremove(COD_Group* group);

    std::unique_ptr<Query> GetQuery() { return std::move(_query); }
    
    std::unique_ptr<ResultQuery> getGroups() override;
    std::unique_ptr<ResultQuery> getGroup(int groupId) override;
private:
    static std::string _database;
    static std::string _table;

    int* _groupIds;
    int* _groupParents;
    std::string _groupNames;

    std::unique_ptr<Query> _query;
    std::shared_ptr<MySQLDBConnection> _dbConnection;
}; 