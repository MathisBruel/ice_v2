#pragma once
#include <iostream>

#include "ContentOpsDomain/COD_Sync.h"
#include "Query.h"
#include "ResultQuery.h"
#include <memory>

class MySQLSyncRepo : public COD_SyncRepo
{
public:
    void Create(COD_Sync* sync) override { _query = MySQLcreate(sync); }
    void Read(COD_Sync* sync) override { _query = MySQLread(sync); }
    void Update(COD_Sync* sync) override { _query = MySQLupdate(sync); }
    void Remove(COD_Sync* sync) override { _query = MySQLremove(sync); }

    std::unique_ptr<Query> MySQLcreate(COD_Sync* sync);
    std::unique_ptr<Query> MySQLread(COD_Sync* sync);
    std::unique_ptr<Query> MySQLread();
    std::unique_ptr<Query> MySQLupdate(COD_Sync* sync);
    std::unique_ptr<Query> MySQLremove(COD_Sync* sync);

    Query* GetQuery() { return _query.get(); }

    std::unique_ptr<ResultQuery> getSyncs() override;

private:
    static std::string _database;
    static std::string _table;

    const int* _syncId;
    std::string _syncPath;

    std::unique_ptr<Query> _query;
};