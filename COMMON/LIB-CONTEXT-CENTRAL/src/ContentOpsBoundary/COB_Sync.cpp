#include "ContentOpsBoundary/COB_Sync.h"
#include <string>

COB_Sync::COB_Sync()
    : COD_Sync() {}

COB_Sync::COB_Sync(int id_serv_pair_config, int id_content, int id_type, int id_localisation, std::string syncPath)
    : COD_Sync() {
    SetSyncId(id_serv_pair_config, id_content, id_type, id_localisation);
    SetSyncInfos(syncPath);
}


COB_Sync::operator std::string() const
{
    std::string xml = "<sync";
    xml += " id_serv_pair_config=\"" + std::to_string(this->GetSyncID()[0]) + "\"";
    xml += " id_content=\"" + std::to_string(this->GetSyncID()[1]) + "\"";
    xml += " id_type=\"" + std::to_string(this->GetSyncID()[2]) + "\"";
    xml += " id_localisation=\"" + std::to_string(this->GetSyncID()[3]) + "\"";
    xml += " path_sync=\"" + this->GetSyncPath() + "\"";
    xml += "/>";
    return xml;
} 