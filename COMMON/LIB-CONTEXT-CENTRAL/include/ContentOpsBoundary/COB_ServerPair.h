#pragma once

#include <string>
#include "ContentOpsDomain/COD_ServerPair.h"

class COB_ServerPair : public COD_ServerPair
{
public:
    COB_ServerPair();
    COB_ServerPair(int id_serv_pair_config, std::string projection_server_ip, std::string auditorium_server_ip, std::string name, int id_default_auditorium, int id_site);
    ~COB_ServerPair();

    operator std::string() const;
}; 