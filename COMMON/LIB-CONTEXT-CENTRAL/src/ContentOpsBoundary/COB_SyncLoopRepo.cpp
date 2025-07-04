#include "ContentOpsBoundary/COB_SyncLoopRepo.h"
#include "ContentOpsBoundary/COB_SyncLoop.h"
#include <stdexcept>
#include <utility>
#include "ResultQuery.h"

COB_SyncLoopRepo::COB_SyncLoopRepo(std::shared_ptr<COD_SyncLoopRepo> syncLoopRepo)
{
    _syncLoopRepo = syncLoopRepo;
}

COB_SyncLoopRepo::~COB_SyncLoopRepo() {}

COB_SyncLoops COB_SyncLoopRepo::GetSyncLoops()
{
    COB_SyncLoops syncLoops;
    // À adapter selon l'API de COD_SyncLoopRepo
    std::unique_ptr<ResultQuery> result = _syncLoopRepo->getSyncLoops();
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get syncLoops: " + (result ? result->getErrorMessage() : "null result"));
    }
    int nbRows = result->getNbRows();
    syncLoops.reserve(nbRows);
    for (int i = 0; i < nbRows; ++i) {
        int* id_serv_pair_config = result->getIntValue(i, "id_serv_pair_config");
        int* id_content = result->getIntValue(i, "id_content");
        int* id_type = result->getIntValue(i, "id_type");
        int* id_localisation = result->getIntValue(i, "id_localisation");
        std::string* path_sync_loop = result->getStringValue(i, "path_sync_loop");
        if (id_serv_pair_config && id_content && id_type && id_localisation && path_sync_loop) {
            COB_SyncLoop syncLoop(*id_serv_pair_config, *id_content, *id_type, *id_localisation, *path_sync_loop);
            syncLoops.emplace_back(syncLoop);
        }
    }
    return std::move(syncLoops);
}

void COB_SyncLoopRepo::Create(COB_SyncLoop* syncLoop) {
    _syncLoopRepo->Create(syncLoop);
}

void COB_SyncLoopRepo::Remove(COB_SyncLoop* syncLoop) {
    _syncLoopRepo->Remove(syncLoop);
}

void COB_SyncLoopRepo::Update(COB_SyncLoop* syncLoop) {
    _syncLoopRepo->Update(syncLoop);
}

Query* COB_SyncLoopRepo::GetQuery() const {
    return static_cast<MySQLSyncLoopRepo*>(_syncLoopRepo.get())->GetQuery();
} 