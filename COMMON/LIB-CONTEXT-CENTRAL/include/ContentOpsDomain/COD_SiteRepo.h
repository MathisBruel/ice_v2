#pragma once

#include "ContentOpsDomain/COD_Site.h"
class COD_SiteRepo
{
public:
    virtual void Create(COD_Site* site) = 0;
    virtual void Read(COD_Site* site) = 0;
    virtual void Update(COD_Site* site) = 0;
    virtual void Remove(COD_Site* site) = 0;
};