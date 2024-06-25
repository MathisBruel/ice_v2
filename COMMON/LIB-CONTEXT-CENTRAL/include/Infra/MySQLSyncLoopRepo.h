#include <iostream>

#include "SyncLoopRepo.h"
#include "Query.h"

class MySQLSyncLoopRepo : public SyncLoopRepo
{
public:
    MySQLSyncLoopRepo();
    ~MySQLSyncLoopRepo();

    Query* create(SyncLoop* syncloop);
    Query* read(SyncLoop* syncloop);
    Query* update(SyncLoop* syncloop);
    Query* remove(SyncLoop* syncloop);

private:
    static std::string database;
    static std::string table;
};