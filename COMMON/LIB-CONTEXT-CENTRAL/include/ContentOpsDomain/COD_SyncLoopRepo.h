#pragma once

#include "ContentOpsDomain/COD_SyncLoop.h"
#include "ResultQuery.h"
class COD_SyncLoop;
class COD_SyncLoopRepo
{
public:
    virtual void Create(COD_SyncLoop* syncloop) = 0;
    virtual void Read(COD_SyncLoop* syncloop) = 0;
    virtual void Update(COD_SyncLoop* syncloop) = 0;
    virtual void Remove(COD_SyncLoop* syncloop) = 0;
    virtual std::unique_ptr<ResultQuery> getSyncLoops() = 0;
    virtual std::unique_ptr<ResultQuery> getSyncLoopsByRelease(int contentId, int typeId, int localisationId) = 0;
};