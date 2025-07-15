#pragma once
#include <iostream>

#include "ContentOpsDomain/COD_ReleaseRepo.h"
#include "Query.h"

class MySQLReleaseRepo : public COD_ReleaseRepo
{
public:
    void Create(COD_Releases* release) override;
    void Read(COD_Releases* release) override;
    void Update(COD_Releases* release) override;
    void Remove(COD_Releases* release) override;

    std::unique_ptr<ResultQuery> getReleases() override;
    std::unique_ptr<ResultQuery> getReleases(int contentId);
    std::unique_ptr<ResultQuery> getRelease(int contentId, int typeId, int localisationId) override;

    std::unique_ptr<Query> MySQLcreate(COD_Releases* release);
    std::unique_ptr<Query> MySQLread(COD_Releases* release);
    std::unique_ptr<Query> MySQLupdate(COD_Releases* release);
    std::unique_ptr<Query> MySQLremove(COD_Releases* release);

    Query* GetQuery() { return _query.get(); }
private:
    static std::string _database;
    static std::string _table;

    int _releaseIds[3];
    std::string _CPLRefPath;
    std::string _CISPath;
    std::string _SyncLoopPath;
    std::unique_ptr<Query> _query;
};