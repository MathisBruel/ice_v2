#pragma once
#include <memory>
#include "ContentOpsDomain/COD_SyncRepo.h"
#include "ContentOpsBoundary/COB_Syncs.h"
#include "ContentOpsInfra/MySQLSyncRepo.h"

class COB_SyncRepo
{
public:
    COB_SyncRepo() : COB_SyncRepo(std::make_shared<MySQLSyncRepo>()) {}
    COB_SyncRepo(std::shared_ptr<COD_SyncRepo> syncRepo);
    ~COB_SyncRepo();

    COB_Syncs GetSyncs();
    void Create(COB_Sync* sync);
    void Remove(COB_Sync* sync);
    void Update(COB_Sync* sync);
    Query* GetQuery() const;

private:
    std::shared_ptr<COD_SyncRepo> _syncRepo;
}; 