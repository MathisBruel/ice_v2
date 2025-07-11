#include "ContentOpsBoundary/COB_ServerPairRepo.h"
#include "Query.h"

COB_ServerPairRepo::COB_ServerPairRepo(std::shared_ptr<COD_ServerPairRepo> serverPairRepo)
    : _serverPairRepo(serverPairRepo)
{
}

COB_ServerPairRepo::~COB_ServerPairRepo()
{
}

COB_ServerPairs COB_ServerPairRepo::GetServerPairs()
{
    COB_ServerPairs serverPairs;
    
    auto result = _serverPairRepo->getServerPairs();
    if (result && result->isValid()) {
        for (int i = 0; i < result->getNbRows(); i++) {
            COB_ServerPair serverPair(
                *result->getIntValue(i, "id_serv_pair_config"),
                *result->getStringValue(i, "projection_server_ip"),
                *result->getStringValue(i, "auditorium_server_ip"),
                *result->getStringValue(i, "name"),
                *result->getIntValue(i, "id_default_auditorium"),
                *result->getIntValue(i, "id_site")
            );
            serverPairs.push_back(serverPair);
        }
    }
    
    return serverPairs;
}

COB_ServerPairs COB_ServerPairRepo::GetServerPairsBySite(int id_site)
{
    COB_ServerPairs serverPairs;
    
    auto result = _serverPairRepo->getServerPairsBySite(id_site);
    if (result && result->isValid()) {
        for (int i = 0; i < result->getNbRows(); i++) {
            COB_ServerPair serverPair(
                *result->getIntValue(i, "id_serv_pair_config"),
                *result->getStringValue(i, "projection_server_ip"),
                *result->getStringValue(i, "auditorium_server_ip"),
                *result->getStringValue(i, "name"),
                *result->getIntValue(i, "id_default_auditorium"),
                *result->getIntValue(i, "id_site")
            );
            serverPairs.push_back(serverPair);
        }
    }
    
    return serverPairs;
}

COB_ServerPair COB_ServerPairRepo::GetServerPair(int id_serv_pair_config)
{
    auto result = _serverPairRepo->getServerPair(id_serv_pair_config);
    if (result && result->isValid() && result->getNbRows() > 0) {
        return COB_ServerPair(
            *result->getIntValue(0, "id_serv_pair_config"),
            *result->getStringValue(0, "projection_server_ip"),
            *result->getStringValue(0, "auditorium_server_ip"),
            *result->getStringValue(0, "name"),
            *result->getIntValue(0, "id_default_auditorium"),
            *result->getIntValue(0, "id_site")
        );
    }
    
    return COB_ServerPair(); // Retourne un ServerPair vide si non trouvé
}

std::unique_ptr<COB_ServerPair> COB_ServerPairRepo::Create(int id_serv_pair_config, std::string projection_server_ip, std::string auditorium_server_ip, std::string name, int id_default_auditorium, int id_site)
{
    auto serverPair = std::make_unique<COB_ServerPair>(id_serv_pair_config, projection_server_ip, auditorium_server_ip, name, id_default_auditorium, id_site);
    _serverPairRepo->Create(serverPair.get());
    return serverPair;
}

void COB_ServerPairRepo::Remove(COB_ServerPair* serverPair)
{
    _serverPairRepo->Remove(serverPair);
}

void COB_ServerPairRepo::Update(COB_ServerPair* serverPair)
{
    _serverPairRepo->Update(serverPair);
}

Query* COB_ServerPairRepo::GetQuery() const
{
    // Cette méthode pourrait être implémentée si nécessaire
    // Pour le moment, on retourne nullptr
    return nullptr;
} 