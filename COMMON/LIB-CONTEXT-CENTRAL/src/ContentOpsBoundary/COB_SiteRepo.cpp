#include "ContentOpsBoundary/COB_SiteRepo.h"
#include <stdexcept>
#include <utility> // Pour std::move
#include "ContentOpsBoundary/COB_Site.h"
#include "ResultQuery.h"
#include "ContentOpsInfra/MySQLSiteRepo.h"

COB_SiteRepo::COB_SiteRepo(COD_SiteRepo* siteRepo)
{
    _siteRepo = siteRepo;
}

COB_SiteRepo::~COB_SiteRepo()
{
    delete _siteRepo;
}


std::vector<COB_Site> COB_SiteRepo::GetSites()
{
    std::vector<COB_Site> sites;
    ResultQuery* result = _siteRepo->getSites();
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get sites : " + std::string(result->getErrorMessage())); 
    }
    int nbRows = result->getNbRows();
    sites.reserve(nbRows);
    
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id_site");
        std::string* name = result->getStringValue(i, "name");
        int* group = result->getIntValue(i, "id_group");
        int* connection = result->getIntValue(i, "id_connection");
        if (id && name && group && connection) {
            sites.emplace_back();
            COB_Site& site = sites.back();
            site.SetSiteId(*id);
            site.SetDatas(*name, *group, *connection); 
        }
    }
    delete result;
    return std::move(sites); 
}

