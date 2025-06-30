#pragma once

#include "ContentOpsDomain/COD_Release.h"
#include "ResultQuery.h"

class COD_ReleaseRepo
{
public:
    virtual void Create(COD_Releases* release) = 0;
    virtual void Read(COD_Releases* release) = 0;
    virtual void Update(COD_Releases* release) = 0;
    virtual void Remove(COD_Releases* release) = 0;

    virtual std::unique_ptr<ResultQuery> getReleases() = 0;
    virtual std::unique_ptr<ResultQuery> getReleases(int contentId) = 0;
    virtual std::unique_ptr<ResultQuery> getRelease(int contentId, int typeId, int localisationId) = 0;
};