#pragma once

#include "ResultQuery.h"

class COD_Sync;
class COD_SyncRepo
{
public:
    virtual void Create(COD_Sync* sync) = 0;
    virtual void Read(COD_Sync* sync) = 0;
    virtual void Update(COD_Sync* sync) = 0;
    virtual void Remove(COD_Sync* sync) = 0;
    virtual std::unique_ptr<ResultQuery> getSyncs() = 0;
};