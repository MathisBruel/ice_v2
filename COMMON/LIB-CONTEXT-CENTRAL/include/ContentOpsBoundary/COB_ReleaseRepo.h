#include <memory>
#pragma once

#include "ContentOpsDomain/COD_ReleaseRepo.h"
#include "ContentOpsBoundary/COB_Release.h"
#include "ContentOpsBoundary/COB_Releases.h"
#include "ContentOpsInfra/MySQLReleaseRepo.h"

class COB_ReleaseRepo
{
public:
    COB_ReleaseRepo() : COB_ReleaseRepo(std::make_shared<MySQLReleaseRepo>()) {}
    COB_ReleaseRepo(std::shared_ptr<COD_ReleaseRepo> releaseRepo);
    ~COB_ReleaseRepo();

    COB_Releases GetReleases();
    COB_Releases GetReleases(int contentId);
    COB_Release GetRelease(int contentId, int typeId, int localisationId);
    void Create(COB_Release* release);
    void Remove(COB_Release* release);
    void Update(COB_Release* release);
    Query* GetQuery() const;

private:
    std::shared_ptr<COD_ReleaseRepo> _releaseRepo;
};