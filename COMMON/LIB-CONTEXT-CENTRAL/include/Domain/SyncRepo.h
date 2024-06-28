#pragma once

#include "Domain/Sync.h"
class SyncRepo
{
public:
    SyncRepo();
    ~SyncRepo();

    virtual void Create(Sync* sync) = 0;
    virtual void Read(Sync* sync) = 0;
    virtual void Update(Sync* sync) = 0;
    virtual void Remove(Sync* sync) = 0;
};