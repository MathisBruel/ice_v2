#include "ContentOpsBoundary/COB_SyncRepo.h"
#include "ContentOpsBoundary/COB_Sync.h"
#include <stdexcept>
#include <utility>
#include "ResultQuery.h"

COB_SyncRepo::COB_SyncRepo(std::shared_ptr<COD_SyncRepo> syncRepo)
{
    _syncRepo = syncRepo;
}

COB_SyncRepo::~COB_SyncRepo() {}

COB_Syncs COB_SyncRepo::GetSyncs()
{
    COB_Syncs syncs;
    // À adapter selon l'API de COD_SyncRepo
    std::unique_ptr<ResultQuery> result = _syncRepo->getSyncs();
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get syncs: " + (result ? result->getErrorMessage() : "null result"));
    }
    int nbRows = result->getNbRows();
    syncs.reserve(nbRows);
    for (int i = 0; i < nbRows; ++i) {
        int* id_serv_pair_config = result->getIntValue(i, "id_serv_pair_config");
        int* id_content = result->getIntValue(i, "id_content");
        int* id_type = result->getIntValue(i, "id_type");
        int* id_localisation = result->getIntValue(i, "id_localisation");
        std::string* path_sync = result->getStringValue(i, "path_sync");
        if (id_serv_pair_config && id_content && id_type && id_localisation && path_sync) {
            COB_Sync sync(*id_serv_pair_config, *id_content, *id_type, *id_localisation, *path_sync);
            syncs.emplace_back(sync);
        }
    }
    return std::move(syncs);
}

void COB_SyncRepo::Create(COB_Sync* sync) {
    _syncRepo->Create(sync);
}

void COB_SyncRepo::Remove(COB_Sync* sync) {
    _syncRepo->Remove(sync);
}

void COB_SyncRepo::Update(COB_Sync* sync) {
    _syncRepo->Update(sync);
}

Query* COB_SyncRepo::GetQuery() const {
    return static_cast<MySQLSyncRepo*>(_syncRepo.get())->GetQuery();
} 