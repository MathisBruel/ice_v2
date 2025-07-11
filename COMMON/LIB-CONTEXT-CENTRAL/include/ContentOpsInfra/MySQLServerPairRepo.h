#pragma once

#include <memory>
#include "ContentOpsDomain/COD_ServerPairRepo.h"
#include "ContentOpsInfra/MySQLDBConnection.h"
#include "Query.h"
#include "ResultQuery.h"

class MySQLServerPairRepo : public COD_ServerPairRepo
{
public:
    MySQLServerPairRepo();
    MySQLServerPairRepo(MySQLDBConnection* connection);
    ~MySQLServerPairRepo();

    void Create(COD_ServerPair* serverPair) override;
    void Read(COD_ServerPair* serverPair) override;
    void Update(COD_ServerPair* serverPair) override;
    void Remove(COD_ServerPair* serverPair) override;
    
    std::unique_ptr<ResultQuery> getServerPairs() override;
    std::unique_ptr<ResultQuery> getServerPair(int id_serv_pair_config) override;
    std::unique_ptr<ResultQuery> getServerPairsBySite(int id_site) override;

private:
    MySQLDBConnection* _connection;
    bool _ownConnection;
}; 