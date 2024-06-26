#pragma once
#include <iostream>

#include "Domain/SyncLoopRepo.h"
#include "Query.h"

class MySQLSyncLoopRepo : public SyncLoopRepo
{
public:
    MySQLSyncLoopRepo();
    ~MySQLSyncLoopRepo();

    void create(SyncLoop* syncloop) { MySQLcreate(syncloop); }
    void read(SyncLoop* syncloop) { MySQLread(syncloop); }
    void update(SyncLoop* syncloop) { MySQLupdate(syncloop); }
    void remove(SyncLoop* syncloop) { MySQLremove(syncloop); }

    Query* MySQLcreate(SyncLoop* syncloop);
    Query* MySQLread(SyncLoop* syncloop);
    Query* MySQLupdate(SyncLoop* syncloop);
    Query* MySQLremove(SyncLoop* syncloop);

private:
    static std::string database;
    static std::string table;
};