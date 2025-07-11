#pragma once
#include <memory>
#include "ContentOpsBoundary/COB_SyncLoops.h"
#include "ContentOpsInfra/MySQLSyncLoopRepo.h"

class COB_SyncLoopRepo
{
public:
    COB_SyncLoopRepo(std::shared_ptr<MySQLSyncLoopRepo> syncLoopRepo) : _syncLoopRepo(syncLoopRepo) {}
    COB_SyncLoopRepo() : _syncLoopRepo(std::make_shared<MySQLSyncLoopRepo>()) {}
    ~COB_SyncLoopRepo();

    COB_SyncLoops GetSyncLoops();
    COB_SyncLoops GetSyncLoopsByRelease(int contentId, int typeId, int localisationId);
    void Create(COB_SyncLoop* syncLoop);
    void Remove(COB_SyncLoop* syncLoop);
    void Update(COB_SyncLoop* syncLoop);
    Query* GetQuery() const;

private:
    std::shared_ptr<MySQLSyncLoopRepo> _syncLoopRepo;
}; 