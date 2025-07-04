#pragma once
#include <memory>
#include "ContentOpsDomain/COD_SyncLoopRepo.h"
#include "ContentOpsBoundary/COB_SyncLoops.h"
#include "ContentOpsInfra/MySQLSyncLoopRepo.h"

class COB_SyncLoopRepo
{
public:
    COB_SyncLoopRepo(std::shared_ptr<COD_SyncLoopRepo> syncLoopRepo);
    COB_SyncLoopRepo() : COB_SyncLoopRepo(std::make_shared<MySQLSyncLoopRepo>()) {}
    ~COB_SyncLoopRepo();

    COB_SyncLoops GetSyncLoops();
    void Create(COB_SyncLoop* syncLoop);
    void Remove(COB_SyncLoop* syncLoop);
    void Update(COB_SyncLoop* syncLoop);
    Query* GetQuery() const;

private:
    std::shared_ptr<COD_SyncLoopRepo> _syncLoopRepo;
}; 