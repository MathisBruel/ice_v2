#include "ContentOpsDomain/COD_ServerPair.h"
#include "ContentOpsDomain/COD_ServerPairRepo.h"

COD_ServerPair::COD_ServerPair()
{
    _id_serv_pair_config = -1;
    _projection_server_ip = "";
    _auditorium_server_ip = "";
    _name = "";
    _id_default_auditorium = -1;
    _id_site = -1;
}

COD_ServerPair::COD_ServerPair(int id_serv_pair_config, std::string projection_server_ip, std::string auditorium_server_ip, std::string name, int id_default_auditorium, int id_site)
{
    _id_serv_pair_config = id_serv_pair_config;
    _projection_server_ip = projection_server_ip;
    _auditorium_server_ip = auditorium_server_ip;
    _name = name;
    _id_default_auditorium = id_default_auditorium;
    _id_site = id_site;
}

COD_ServerPair::~COD_ServerPair()
{
}

void COD_ServerPair::SetServerPairId(int id_serv_pair_config)
{
    this->_id_serv_pair_config = id_serv_pair_config;
}

void COD_ServerPair::SetDatas(std::string projection_server_ip, std::string auditorium_server_ip, std::string name, int id_default_auditorium, int id_site)
{
    this->_projection_server_ip = projection_server_ip;
    this->_auditorium_server_ip = auditorium_server_ip;
    this->_name = name;
    this->_id_default_auditorium = id_default_auditorium;
    this->_id_site = id_site;
} 