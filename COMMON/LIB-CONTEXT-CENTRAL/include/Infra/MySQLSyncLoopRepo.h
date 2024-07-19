#pragma once
#include <iostream>

#include "Domain/SyncLoopRepo.h"
#include "Query.h"

class MySQLSyncLoopRepo : public SyncLoopRepo
{
public:
    void Create(SyncLoop* syncloop) override { _query = MySQLcreate(syncloop); }
    void Read(SyncLoop* syncloop) override { _query = MySQLread(syncloop); }
    void Update(SyncLoop* syncloop) override { _query = MySQLupdate(syncloop); }
    void Remove(SyncLoop* syncloop) override { _query = MySQLremove(syncloop); }

    Query* MySQLcreate(SyncLoop* syncloop);
    Query* MySQLread(SyncLoop* syncloop);
    Query* MySQLupdate(SyncLoop* syncloop);
    Query* MySQLremove(SyncLoop* syncloop);

    Query* GetQuery() { return _query; }
private:
    static std::string _database;
    static std::string _table;
    
    int* _syncloopIds;
    std::string _syncLoopPath;

    Query* _query;
};