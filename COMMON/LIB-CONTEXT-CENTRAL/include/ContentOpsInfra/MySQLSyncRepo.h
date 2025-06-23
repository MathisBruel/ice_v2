#pragma once
#include <iostream>

#include "ContentOpsDomain/SyncRepo.h"
#include "Query.h"

class MySQLSyncRepo : public SyncRepo
{
public:
    void Create(Sync* sync) override { _query = MySQLcreate(sync); }
    void Read(Sync* sync) override { _query = MySQLread(sync); }
    void Update(Sync* sync) override { _query = MySQLupdate(sync); }
    void Remove(Sync* sync) override { _query = MySQLremove(sync); }

    Query* MySQLcreate(Sync* sync);
    Query* MySQLread(Sync* sync);
    Query* MySQLupdate(Sync* sync);
    Query* MySQLremove(Sync* sync);

    Query* GetQuery() { return _query; }
private:
    static std::string _database;
    static std::string _table;

    int* _syncId;
    std::string _syncPath;

    Query* _query;
};