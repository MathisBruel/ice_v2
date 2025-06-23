#pragma once

#include "ContentOpsDomain/Sync.h"
class SyncRepo
{
public:
    virtual void Create(Sync* sync) = 0;
    virtual void Read(Sync* sync) = 0;
    virtual void Update(Sync* sync) = 0;
    virtual void Remove(Sync* sync) = 0;
};