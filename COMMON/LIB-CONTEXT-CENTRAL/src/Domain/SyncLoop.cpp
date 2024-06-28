#include "Domain/SyncLoop.h"
#include "Domain/SyncLoopRepo.h"
SyncLoop::SyncLoop()
{
    _syncLoopId[0] = -1;
    _syncLoopId[1] = -1;
    _syncLoopId[2] = -1;
    _syncLoopId[3] = -1;
    _SyncLoopPath = "";
}
SyncLoop::~SyncLoop() {}
void SyncLoop::SetSyncLoopId(int id_serv_pair_config, int id_movie, int id_type, int id_localisation)
{
    _syncLoopId[0] = id_serv_pair_config;
    _syncLoopId[1] = id_movie;
    _syncLoopId[2] = id_type;
    _syncLoopId[3] = id_localisation;
}
void SyncLoop::SetSyncLoopInfos(std::string SyncLoopPath)
{
    this->_SyncLoopPath = SyncLoopPath;
}