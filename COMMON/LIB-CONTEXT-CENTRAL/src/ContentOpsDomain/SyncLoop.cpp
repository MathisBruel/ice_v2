#include "ContentOpsDomain/SyncLoop.h"
#include "ContentOpsDomain/SyncLoopRepo.h"
SyncLoop::SyncLoop()
{
    _syncLoopId[0] = -1;
    _syncLoopId[1] = -1;
    _syncLoopId[2] = -1;
    _syncLoopId[3] = -1;
    _SyncLoopPath = "";
}
SyncLoop::~SyncLoop() {}
void SyncLoop::SetSyncLoopId(int id_serv_pair_config, int id_content, int id_type, int id_localisation)
{
    _syncLoopId[0] = id_serv_pair_config;
    _syncLoopId[1] = id_content;
    _syncLoopId[2] = id_type;
    _syncLoopId[3] = id_localisation;
}
void SyncLoop::SetSyncLoopInfos(std::string SyncLoopPath)
{
    this->_SyncLoopPath = SyncLoopPath;
}
std::string SyncLoop::toXmlString()
{
    std::string xml = "<syncLoop";
    xml += " id_serv_pair_config=\"" + std::to_string(this->_syncLoopId[0]) + "\"";
    xml += " id_content=\"" + std::to_string(this->_syncLoopId[1]) + "\"";
    xml += " id_type=\"" + std::to_string(this->_syncLoopId[2]) + "\"";
    xml += " id_localisation=\"" + std::to_string(this->_syncLoopId[3]) + "\"";
    xml += " path_sync_loop=\"" + this->_SyncLoopPath + "\"";
    xml += "/>";
    return xml;
}