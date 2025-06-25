#include "ContentOpsBoundary/COB_SiteRepo.h"

std::vector<COD_Site*> COB_SiteRepo::GetSites(MySQLDBConnection* dbConn)
{
    std::vector<COD_Site*> sites;
    // Créer la requête pour récupérer tous les sites
    Query* query = MySQLread();
    // Exécuter la requête
    ResultQuery* result = dbConn->ExecuteQuery(query);
    if (!result || !result->isValid()) {
        delete query;
        return sites;
    }
    int nbRows = result->getNbRows();
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id_site");
        std::string* name = result->getStringValue(i, "name");
        int* group = result->getIntValue(i, "id_group");
        int* connection = result->getIntValue(i, "id_connection");
        if (id && name && group && connection) {
            COD_Site* site = new COD_Site();
            site->SetSiteId(*id);
            site->SetDatas(*name, *group, *connection);
            sites.push_back(site);
        }
    }
    delete result;
    delete query;
    return sites;
}

