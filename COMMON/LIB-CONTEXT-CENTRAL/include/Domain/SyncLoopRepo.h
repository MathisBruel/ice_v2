#pragma once

#include "Domain/SyncLoop.h"
class SyncLoopRepo
{
public:
    virtual void Create(SyncLoop* syncloop) = 0;
    virtual void Read(SyncLoop* syncloop) = 0;
    virtual void Update(SyncLoop* syncloop) = 0;
    virtual void Remove(SyncLoop* syncloop) = 0;
};