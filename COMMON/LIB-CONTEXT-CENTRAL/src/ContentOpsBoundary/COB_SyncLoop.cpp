#include "ContentOpsBoundary/COB_SyncLoop.h"
#include <string>

COB_SyncLoop::COB_SyncLoop()
    : COD_SyncLoop() {}

COB_SyncLoop::COB_SyncLoop(int id_serv_pair_config, int id_content, int id_type, int id_localisation, std::string syncLoopPath)
    : COD_SyncLoop() {
    SetSyncLoopId(id_serv_pair_config, id_content, id_type, id_localisation);
    SetSyncLoopInfos(syncLoopPath);
}

COB_SyncLoop::COB_SyncLoop(const COD_SyncLoop& src)
    : COD_SyncLoop() {
    const int* srcId = src.GetSyncLoopId();
    SetSyncLoopId(srcId[0], srcId[1], srcId[2], srcId[3]);
    SetSyncLoopInfos(src.GetSyncLoopPath());
}

COB_SyncLoop::operator std::string() const
{
    std::string xml = "<syncLoop";
    xml += " id_serv_pair_config=\"" + std::to_string(this->GetSyncLoopId()[0]) + "\"";
    xml += " id_content=\"" + std::to_string(this->GetSyncLoopId()[1]) + "\"";
    xml += " id_type=\"" + std::to_string(this->GetSyncLoopId()[2]) + "\"";
    xml += " id_localisation=\"" + std::to_string(this->GetSyncLoopId()[3]) + "\"";
    xml += " path_sync_loop=\"" + this->GetSyncLoopPath() + "\"";
    xml += "/>";
    return xml;
} 