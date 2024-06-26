#pragma once

#include "Domain/SyncLoop.h"
class SyncLoopRepo
{
public:
    SyncLoopRepo();
    ~SyncLoopRepo();

    virtual void create(SyncLoop* syncloop);
    virtual void read(SyncLoop* syncloop);
    virtual void update(SyncLoop* syncloop);
    virtual void remove(SyncLoop* syncloop);
};