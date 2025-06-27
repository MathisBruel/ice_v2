#include "ContentOpsBoundary/COB_SiteRepo.h"
#include "ContentOpsBoundary/COB_Sites.h"
#include <stdexcept>
#include <utility> // Pour std::move
#include "ContentOpsBoundary/COB_Site.h"
#include "ResultQuery.h"
#include "ContentOpsInfra/MySQLSiteRepo.h"

COB_SiteRepo::COB_SiteRepo(std::shared_ptr<COD_SiteRepo> siteRepo)
{
    _siteRepo = siteRepo;
}

COB_SiteRepo::~COB_SiteRepo()
{
}

COB_Sites COB_SiteRepo::GetSites(int groupId)
{
    COB_Sites sites;
    std::unique_ptr<ResultQuery> result = _siteRepo->getSites(groupId);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get sites for group " + std::to_string(groupId) + ": " + (result ? result->getErrorMessage() : "null result")); 
    }
    int nbRows = result->getNbRows();
    sites.reserve(nbRows);
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id_site");
        std::string* name = result->getStringValue(i, "name");
        int* group = result->getIntValue(i, "id_group");
        int* connection = result->getIntValue(i, "id_connection");
        if (id && name) {
            int groupValue = group ? *group : -1;
            int connectionValue = connection ? *connection : -1;
            COB_Site site(*id, *name, groupValue, connectionValue);
            sites.emplace_back(site);
        }
    }
    return std::move(sites); 
}

