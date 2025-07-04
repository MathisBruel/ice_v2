#pragma once
#include <string>
#include "ContentOpsDomain/COD_SyncLoop.h"

class COB_SyncLoop : public COD_SyncLoop
{
public:
    COB_SyncLoop();
    COB_SyncLoop(int id_serv_pair_config, int id_content, int id_type, int id_localisation, std::string syncLoopPath);
    COB_SyncLoop(const COD_SyncLoop& src);
    operator std::string() const;
}; 