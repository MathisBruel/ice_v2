#include "ContentOpsDomain/Sync.h"
#include "ContentOpsDomain/SyncRepo.h"
Sync::Sync()
{
    _syncId[0] = -1;
    _syncId[1] = -1;
    _syncId[2] = -1;
    _syncId[3] = -1;
    _syncPath = "";
}
void Sync::SetSyncId(int id_serv_pair_config, int id_content, int id_type, int id_localisation)
{
    _syncId[0] = id_serv_pair_config;
    _syncId[1] = id_content;
    _syncId[2] = id_type;
    _syncId[3] = id_localisation;
}
void Sync::SetSyncInfos(std::string SyncPath)
{
    this->_syncPath = SyncPath;
}
std::string Sync::toXmlString()
{
    std::string xml = "<sync";
    xml += " id_serv_pair_config=\"" + std::to_string(this->_syncId[0]) + "\"";
    xml += " id_content=\"" + std::to_string(this->_syncId[1]) + "\"";
    xml += " id_type=\"" + std::to_string(this->_syncId[2]) + "\"";
    xml += " id_localisation=\"" + std::to_string(this->_syncId[3]) + "\"";
    xml += " path_sync=\"" + this->_syncPath + "\"";
    xml += "/>";
    return xml;
}