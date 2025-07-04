#include "ContentOpsDomain/COD_Sync.h"
#include "ContentOpsDomain/COD_SyncRepo.h"
COD_Sync::COD_Sync()
{
    _syncId[0] = -1;
    _syncId[1] = -1;
    _syncId[2] = -1;
    _syncId[3] = -1;
    _syncPath = "";
}
void COD_Sync::SetSyncId(int id_serv_pair_config, int id_content, int id_type, int id_localisation)
{
    _syncId[0] = id_serv_pair_config;
    _syncId[1] = id_content;
    _syncId[2] = id_type;
    _syncId[3] = id_localisation;
}
void COD_Sync::SetSyncInfos(std::string SyncPath)
{
    this->_syncPath = SyncPath;
}