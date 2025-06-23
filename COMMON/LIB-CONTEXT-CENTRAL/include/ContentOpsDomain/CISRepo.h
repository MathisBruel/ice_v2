#pragma once

#include "ContentOpsDomain/CIS.h"
class CISRepo
{
public:
    virtual void Create(CIS* cis) = 0;
    virtual void Read(CIS* cis) = 0;
    virtual void Update(CIS* cis) = 0;
    virtual void Remove(CIS* cis) = 0;
};