#pragma once

#include <vector>
#include "ContentOpsInfra/MySQLSiteRepo.h"
#include "ContentOpsDomain/COD_Site.h"
#include "ContentOpsInfra/MySQLDBConnection.h"

class COB_SiteRepo : public MySQLSiteRepo
{
public: 
    std::vector<COD_Site*> GetSites(MySQLDBConnection* dbConn);
};