#pragma once

#include "ContentOpsDomain/COD_CIS.h"
class CISRepo
{
public:
    virtual void Create(COD_CIS* cis) = 0;
    virtual void Read(COD_CIS* cis) = 0;
    virtual void Update(COD_CIS* cis) = 0;
    virtual void Remove(COD_CIS* cis) = 0;
};