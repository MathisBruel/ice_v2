#pragma once

class COD_Site;
class ResultQuery;

class COD_SiteRepo
{
public:
    virtual void Create(COD_Site* site) = 0;
    virtual void Read(COD_Site* site) = 0;
    virtual void Update(COD_Site* site) = 0;
    virtual void Remove(COD_Site* site) = 0;
    virtual ResultQuery* getSites(int groupId) = 0;
};