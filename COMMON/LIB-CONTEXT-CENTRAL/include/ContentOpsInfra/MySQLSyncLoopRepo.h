#pragma once
#include <iostream>

#include "ContentOpsDomain/COD_SyncLoop.h"
#include "Query.h"

class MySQLSyncLoopRepo : public COD_SyncLoopRepo
{
public:
    void Create(COD_SyncLoop* syncloop) override { _query = MySQLcreate(syncloop); }
    void Read(COD_SyncLoop* syncloop) override { _query = MySQLread(syncloop); }
    void Update(COD_SyncLoop* syncloop) override { _query = MySQLupdate(syncloop); }
    void Remove(COD_SyncLoop* syncloop) override { _query = MySQLremove(syncloop); }

    Query* MySQLcreate(COD_SyncLoop* syncloop);
    Query* MySQLread(COD_SyncLoop* syncloop);
    Query* MySQLupdate(COD_SyncLoop* syncloop);
    Query* MySQLremove(COD_SyncLoop* syncloop);

    Query* GetQuery() { return _query; }
private:
    static std::string _database;
    static std::string _table;
    
    int* _syncloopIds;
    std::string _syncLoopPath;

    Query* _query;
};