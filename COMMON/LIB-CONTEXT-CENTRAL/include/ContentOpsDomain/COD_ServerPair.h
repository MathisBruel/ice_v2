#pragma once
#include <iostream>
#include <string>

class COD_ServerPairRepo;

class COD_ServerPair
{
public:
    COD_ServerPair();
    COD_ServerPair(int id_serv_pair_config, std::string projection_server_ip, std::string auditorium_server_ip, std::string name, int id_default_auditorium, int id_site);
    ~COD_ServerPair();

    void SetServerPairId(int id_serv_pair_config);
    void SetDatas(std::string projection_server_ip, std::string auditorium_server_ip, std::string name, int id_default_auditorium, int id_site);

    int GetServerPairId() const { return this->_id_serv_pair_config; }
    std::string GetProjectionServerIp() const { return this->_projection_server_ip; }
    std::string GetAuditoriumServerIp() const { return this->_auditorium_server_ip; }
    std::string GetName() const { return this->_name; }
    int GetIdDefaultAuditorium() const { return this->_id_default_auditorium; }
    int GetIdSite() const { return this->_id_site; }

    int* GetServerPairIdPtr() { return &this->_id_serv_pair_config; }
    std::string* GetProjectionServerIpPtr() { return &this->_projection_server_ip; }
    std::string* GetAuditoriumServerIpPtr() { return &this->_auditorium_server_ip; }
    std::string* GetNamePtr() { return &this->_name; }
    int* GetIdDefaultAuditoriumPtr() { return &this->_id_default_auditorium; }
    int* GetIdSitePtr() { return &this->_id_site; }

private:
    COD_ServerPairRepo* _serverPairRepo;
    
    int _id_serv_pair_config;
    std::string _projection_server_ip;
    std::string _auditorium_server_ip;
    std::string _name;
    int _id_default_auditorium;
    int _id_site;
}; 