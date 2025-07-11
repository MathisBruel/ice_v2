#pragma once

#include <vector>
#include <memory>
#include "ContentOpsBoundary/COB_ServerPair.h"
#include "ContentOpsBoundary/COB_ServerPairs.h"
#include "ContentOpsDomain/COD_ServerPairRepo.h"
#include "ContentOpsInfra/MySQLServerPairRepo.h"

class COB_ServerPairRepo
{
public: 
    COB_ServerPairRepo(std::shared_ptr<COD_ServerPairRepo> serverPairRepo);
    COB_ServerPairRepo() : COB_ServerPairRepo(std::make_shared<MySQLServerPairRepo>()) {}
    ~COB_ServerPairRepo();

    COB_ServerPairs GetServerPairs();
    COB_ServerPairs GetServerPairsBySite(int id_site);
    COB_ServerPair GetServerPair(int id_serv_pair_config);
    std::unique_ptr<COB_ServerPair> Create(int id_serv_pair_config, std::string projection_server_ip, std::string auditorium_server_ip, std::string name, int id_default_auditorium, int id_site);
    void Remove(COB_ServerPair* serverPair);
    void Update(COB_ServerPair* serverPair);
    Query* GetQuery() const;

private:
    std::shared_ptr<COD_ServerPairRepo> _serverPairRepo;
}; 