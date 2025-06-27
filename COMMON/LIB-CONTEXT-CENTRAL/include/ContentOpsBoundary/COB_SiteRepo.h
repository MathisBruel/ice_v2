#pragma once

#include <vector>
#include "ContentOpsBoundary/COB_Site.h"
#include "ContentOpsBoundary/COB_Sites.h"
#include "ContentOpsDomain/COD_SiteRepo.h"
#include <memory>

class COB_SiteRepo
{
public: 
    COB_SiteRepo(std::shared_ptr<COD_SiteRepo> siteRepo);
    ~COB_SiteRepo();

    COB_Sites GetSites(int groupId);

private:
    std::shared_ptr<COD_SiteRepo> _siteRepo;
};