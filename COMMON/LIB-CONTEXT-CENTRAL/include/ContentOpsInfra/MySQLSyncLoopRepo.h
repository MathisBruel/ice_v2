#pragma once
#include <iostream>

#include "ContentOpsDomain/COD_SyncLoopRepo.h"
#include "ContentOpsDomain/COD_SyncLoop.h"
#include "Query.h"
#include "ResultQuery.h"
#include <memory>

class MySQLSyncLoopRepo : public COD_SyncLoopRepo
{
public:
    void Create(COD_SyncLoop* syncloop) override { _query = MySQLcreate(syncloop); }
    void Read(COD_SyncLoop* syncloop) override { _query = MySQLread(syncloop); }
    void Update(COD_SyncLoop* syncloop) override { _query = MySQLupdate(syncloop); }
    void Remove(COD_SyncLoop* syncloop) override { _query = MySQLremove(syncloop); }

    std::unique_ptr<Query> MySQLcreate(COD_SyncLoop* syncloop);
    std::unique_ptr<Query> MySQLread(COD_SyncLoop* syncloop);
    std::unique_ptr<Query> MySQLread();
    std::unique_ptr<Query> MySQLupdate(COD_SyncLoop* syncloop);
    std::unique_ptr<Query> MySQLremove(COD_SyncLoop* syncloop);

    Query* GetQuery() { return _query.get(); }

    std::unique_ptr<ResultQuery> getSyncLoops() override;

private:
    static std::string _database;
    static std::string _table;
    
    const int* _syncloopIds;
    std::string _syncLoopPath;

    std::unique_ptr<Query> _query;
};