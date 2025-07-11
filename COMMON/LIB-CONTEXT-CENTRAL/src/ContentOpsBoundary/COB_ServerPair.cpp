#include "ContentOpsBoundary/COB_ServerPair.h"

COB_ServerPair::COB_ServerPair()
    : COD_ServerPair()
{
}

COB_ServerPair::COB_ServerPair(int id_serv_pair_config, std::string projection_server_ip, std::string auditorium_server_ip, std::string name, int id_default_auditorium, int id_site)
    : COD_ServerPair(id_serv_pair_config, projection_server_ip, auditorium_server_ip, name, id_default_auditorium, id_site)
{
}

COB_ServerPair::~COB_ServerPair()
{
}

COB_ServerPair::operator std::string() const
{
    std::string xml = "<servPair";
    xml += " id_serv_pair_config=\"" + std::to_string(this->GetServerPairId()) + "\"";
    xml += " projection_server_ip=\"" + this->GetProjectionServerIp() + "\"";
    xml += " auditorium_server_ip=\"" + this->GetAuditoriumServerIp() + "\"";
    xml += " name=\"" + this->GetName() + "\"";
    xml += " id_default_auditorium=\"" + std::to_string(this->GetIdDefaultAuditorium()) + "\"";
    xml += " id_site=\"" + std::to_string(this->GetIdSite()) + "\"";
    xml += " />";
    return xml;
} 