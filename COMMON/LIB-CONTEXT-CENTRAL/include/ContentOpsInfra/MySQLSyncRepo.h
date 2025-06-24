#pragma once
#include <iostream>

#include "ContentOpsDomain/COD_Sync.h"
#include "Query.h"

class MySQLSyncRepo : public COD_SyncRepo
{
public:
    void Create(COD_Sync* sync) override { _query = MySQLcreate(sync); }
    void Read(COD_Sync* sync) override { _query = MySQLread(sync); }
    void Update(COD_Sync* sync) override { _query = MySQLupdate(sync); }
    void Remove(COD_Sync* sync) override { _query = MySQLremove(sync); }

    Query* MySQLcreate(COD_Sync* sync);
    Query* MySQLread(COD_Sync* sync);
    Query* MySQLupdate(COD_Sync* sync);
    Query* MySQLremove(COD_Sync* sync);

    Query* GetQuery() { return _query; }
private:
    static std::string _database;
    static std::string _table;

    int* _syncId;
    std::string _syncPath;

    Query* _query;
};