#include "ContentOpsDomain/COD_SyncLoop.h"
#include "ContentOpsDomain/COD_SyncLoopRepo.h"
COD_SyncLoop::COD_SyncLoop()
{
    _syncLoopId[0] = -1;
    _syncLoopId[1] = -1;
    _syncLoopId[2] = -1;
    _syncLoopId[3] = -1;
    _SyncLoopPath = "";
}
COD_SyncLoop::~COD_SyncLoop() {}
void COD_SyncLoop::SetSyncLoopId(int id_serv_pair_config, int id_content, int id_type, int id_localisation)
{
    _syncLoopId[0] = id_serv_pair_config;
    _syncLoopId[1] = id_content;
    _syncLoopId[2] = id_type;
    _syncLoopId[3] = id_localisation;
}
void COD_SyncLoop::SetSyncLoopInfos(std::string SyncLoopPath)
{
    this->_SyncLoopPath = SyncLoopPath;
}