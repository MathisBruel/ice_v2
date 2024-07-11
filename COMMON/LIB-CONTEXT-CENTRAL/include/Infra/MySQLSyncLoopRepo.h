#pragma once
#include <iostream>

#include "Domain/SyncLoopRepo.h"
#include "Query.h"

class MySQLSyncLoopRepo : public SyncLoopRepo
{
public:
    void Create(SyncLoop* syncloop) override { MySQLcreate(syncloop); }
    void Read(SyncLoop* syncloop) override { MySQLread(syncloop); }
    void Update(SyncLoop* syncloop) override { MySQLupdate(syncloop); }
    void Remove(SyncLoop* syncloop) override { MySQLremove(syncloop); }

    Query* MySQLcreate(SyncLoop* syncloop);
    Query* MySQLread(SyncLoop* syncloop);
    Query* MySQLupdate(SyncLoop* syncloop);
    Query* MySQLremove(SyncLoop* syncloop);

private:
    static std::string _database;
    static std::string _table;
};