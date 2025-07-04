#pragma once
#include <string>
#include "ContentOpsDomain/COD_Sync.h"

class COB_Sync : public COD_Sync
{
public:
    COB_Sync();
    COB_Sync(int id_serv_pair_config, int id_content, int id_type, int id_localisation, std::string syncPath);
    COB_Sync(const COD_Sync& src);
    operator std::string() const;
}; 