#pragma once

#include "ResultQuery.h"

class COD_ServerPair;

class COD_ServerPairRepo
{
public:
    virtual ~COD_ServerPairRepo() = default;
    virtual void Create(COD_ServerPair* serverPair) = 0;
    virtual void Read(COD_ServerPair* serverPair) = 0;
    virtual void Update(COD_ServerPair* serverPair) = 0;
    virtual void Remove(COD_ServerPair* serverPair) = 0;
    virtual std::unique_ptr<ResultQuery> getServerPairs() = 0;
    virtual std::unique_ptr<ResultQuery> getServerPair(int id_serv_pair_config) = 0;
    virtual std::unique_ptr<ResultQuery> getServerPairsBySite(int id_site) = 0;
}; 