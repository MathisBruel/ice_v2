#pragma once

#include "Domain/Sync.h"
class SyncRepo
{
public:
    SyncRepo();
    ~SyncRepo();

    virtual void create(Sync* sync);
    virtual void read(Sync* sync);
    virtual void update(Sync* sync);
    virtual void remove(Sync* sync);
};