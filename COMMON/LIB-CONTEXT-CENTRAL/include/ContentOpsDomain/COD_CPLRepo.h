#pragma once

#include "ContentOpsDomain/COD_CPL.h"
class COD_CPLRepo
{
public:
    virtual void Create(COD_CPLRelease* cpl) = 0;
    virtual void Read(COD_CPLRelease* cpl) = 0;
    virtual void Update(COD_CPLRelease* cpl) = 0;
    virtual void Remove(COD_CPLRelease* cpl) = 0;
};