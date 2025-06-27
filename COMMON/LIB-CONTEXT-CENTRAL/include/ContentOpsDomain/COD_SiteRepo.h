#pragma once

#include <memory>

class COD_Site;
class ResultQuery;

class COD_SiteRepo
{
public:
    virtual ~COD_SiteRepo() = default;
    virtual void Create(COD_Site* site) = 0;
    virtual void Read(COD_Site* site) = 0;
    virtual void Update(COD_Site* site) = 0;
    virtual void Remove(COD_Site* site) = 0;
    virtual std::unique_ptr<ResultQuery> getSites(int groupId) = 0;
};