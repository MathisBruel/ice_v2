#pragma once
#include <iostream>

#include "Domain/SyncRepo.h"
#include "Query.h"

class MySQLSyncRepo : public SyncRepo
{
public:
    MySQLSyncRepo();
    ~MySQLSyncRepo();

    void create(Sync* sync) { MySQLcreate(sync); }
    void read(Sync* sync) { MySQLread(sync); }
    void update(Sync* sync) { MySQLupdate(sync); }
    void remove(Sync* sync) { MySQLremove(sync); }

    Query* MySQLcreate(Sync* sync);
    Query* MySQLread(Sync* sync);
    Query* MySQLupdate(Sync* sync);
    Query* MySQLremove(Sync* sync);
private:
    static std::string database;
    static std::string table;
};