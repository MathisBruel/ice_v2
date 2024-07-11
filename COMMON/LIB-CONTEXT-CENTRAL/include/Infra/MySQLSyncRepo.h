#pragma once
#include <iostream>

#include "Domain/SyncRepo.h"
#include "Query.h"

class MySQLSyncRepo : public SyncRepo
{
public:
    void Create(Sync* sync) override { MySQLcreate(sync); }
    void Read(Sync* sync) override { MySQLread(sync); }
    void Update(Sync* sync) override { MySQLupdate(sync); }
    void Remove(Sync* sync) override { MySQLremove(sync); }

    Query* MySQLcreate(Sync* sync);
    Query* MySQLread(Sync* sync);
    Query* MySQLupdate(Sync* sync);
    Query* MySQLremove(Sync* sync);
private:
    static std::string _database;
    static std::string _table;
};