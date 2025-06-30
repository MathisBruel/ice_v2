#include <memory>
#pragma once

#include "ContentOpsDomain/COD_ReleaseRepo.h"
#include "ContentOpsBoundary/COB_Release.h"
#include "ContentOpsBoundary/COB_Releases.h"

class COB_ReleaseRepo
{
public:
    COB_ReleaseRepo(std::shared_ptr<COD_ReleaseRepo> releaseRepo);
    ~COB_ReleaseRepo();

    COB_Releases GetReleases();
    COB_Releases GetReleases(int contentId);
    COB_Release GetRelease(int contentId, int typeId, int localisationId);

private:
    std::shared_ptr<COD_ReleaseRepo> _releaseRepo;
};