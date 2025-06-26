#pragma once

#include <vector>
#include "ContentOpsBoundary/COB_Site.h"
#include "ContentOpsDomain/COD_SiteRepo.h"

class COB_SiteRepo
{
public: 
    COB_SiteRepo(COD_SiteRepo* siteRepo);
    ~COB_SiteRepo();

    std::vector<COB_Site> GetSites(int groupId);

private:
    COD_SiteRepo* _siteRepo;
};