#pragma once

#include "ContentOpsDomain/COD_GroupRepo.h"
#include <iostream>
#include "Query.h"
#include "ResultQuery.h"
#include "ContentOpsDomain/COD_Group.h"
#include "ContentOpsBoundary/COB_Group.h"
#include "ResultQuery.h"
#include "ContentOpsInfra/MySQLDBConnection.h"

class MySQLGroupRepo : public COD_GroupRepo
{
public:
    MySQLGroupRepo();
    virtual ~MySQLGroupRepo();
    
    void Create(COD_Group* group) override;
    void Read(COD_Group* group) override;
    void Update(COD_Group* group) override;
    void Remove(COD_Group* group) override;

    Query* MySQLcreate(COD_Group* group);
    Query* MySQLread(COD_Group* group);
    Query* MySQLread();
    Query* MySQLupdate(COD_Group* group);
    Query* MySQLremove(COD_Group* group);

    Query* GetQuery() { return _query; }

    ResultQuery* getGroups();
    ResultQuery* getGroup(int groupId);
private:
    static std::string _database;
    static std::string _table;

    int* _groupIds;
    int* _groupParents;
    std::string _groupNames;

    Query* _query;
}; 