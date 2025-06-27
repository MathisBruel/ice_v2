#pragma once

#include "ContentOpsDomain/COD_SyncLoop.h"
class COD_SyncLoop;
class COD_SyncLoopRepo
{
public:
    virtual void Create(COD_SyncLoop* syncloop) = 0;
    virtual void Read(COD_SyncLoop* syncloop) = 0;
    virtual void Update(COD_SyncLoop* syncloop) = 0;
    virtual void Remove(COD_SyncLoop* syncloop) = 0;
};