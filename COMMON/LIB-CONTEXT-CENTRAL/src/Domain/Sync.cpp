#include "Domain/Sync.h"
#include "Domain/SyncRepo.h"
Sync::Sync()
{
    _syncId[0] = -1;
    _syncId[1] = -1;
    _syncId[2] = -1;
    _syncId[3] = -1;
    _syncPath = "";
}
Sync::~Sync() {}
void Sync::SetSyncId(int id_serv_pair_config, int id_movie, int id_type, int id_localisation)
{
    _syncId[0] = id_serv_pair_config;
    _syncId[1] = id_movie;
    _syncId[2] = id_type;
    _syncId[3] = id_localisation;
}
void Sync::SetSyncInfos(std::string SyncPath)
{
    this->_syncPath = SyncPath;
}