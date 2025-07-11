#pragma once
#include <iostream>

#include "ContentOpsBoundary/COB_SyncLoop.h"
#include "ContentOpsBoundary/COB_SyncLoops.h"
#include "Query.h"
#include "ResultQuery.h"
#include <memory>

class MySQLSyncLoopRepo
{
public:
    MySQLSyncLoopRepo();
    virtual ~MySQLSyncLoopRepo();

    void Create(COB_SyncLoop* syncLoop);
    void Read(COB_SyncLoop* syncLoop);
    void Update(COB_SyncLoop* syncLoop);
    void Remove(COB_SyncLoop* syncLoop);

    std::unique_ptr<Query> MySQLcreate(COB_SyncLoop* syncLoop);
    std::unique_ptr<Query> MySQLread(COB_SyncLoop* syncLoop);
    std::unique_ptr<Query> MySQLread();
    std::unique_ptr<Query> MySQLupdate(COB_SyncLoop* syncLoop);
    std::unique_ptr<Query> MySQLremove(COB_SyncLoop* syncLoop);

    Query* GetQuery() { return _query.get(); }

    std::unique_ptr<ResultQuery> getSyncLoops();
    std::unique_ptr<ResultQuery> getSyncLoopsByRelease(int contentId, int typeId, int localisationId);

private:
    static std::string _database;
    static std::string _table;
    
    int* _syncLoopIds;
    std::string _syncLoopPath;
    std::unique_ptr<Query> _query;
};