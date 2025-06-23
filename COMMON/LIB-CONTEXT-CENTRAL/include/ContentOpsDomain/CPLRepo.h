#pragma once

#include "ContentOpsDomain/CPL.h"
class CPLRepo
{
public:
    virtual void Create(CPLRelease* cpl) = 0;
    virtual void Read(CPLRelease* cpl) = 0;
    virtual void Update(CPLRelease* cpl) = 0;
    virtual void Remove(CPLRelease* cpl) = 0;
};