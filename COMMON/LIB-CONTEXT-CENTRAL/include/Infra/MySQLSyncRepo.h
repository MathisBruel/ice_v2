#include <iostream>

#include "SyncRepo.h"
#include "Query.h"

class MySQLSyncRepo : public SyncRepo
{
public:
    MySQLSyncRepo();
    ~MySQLSyncRepo();

    Query* create(Sync* sync);
    Query* read(Sync* sync);
    Query* update(Sync* sync);
    Query* remove(Sync* sync);
private:
    static std::string database;
    static std::string table;
};